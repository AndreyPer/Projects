import telebot
import random
from telebot import types
import json
import requests


bot = telebot.TeleBot('6267633596:AAH2cLk7g8z48nfmqcWocBtCUHkTyPnxQ0Q')

with open('questions.json', 'r') as file:
    brands = json.load(file)

user_data = {}


def ask_question(user_id):
    if not user_data[user_id]['remaining_questions']:
        user_data[user_id]['remaining_questions'] = brands[user_data[user_id][
            'selected_brand']][:]
        random.shuffle(user_data[user_id]['remaining_questions'])

    question_data = user_data[user_id]['remaining_questions'].pop(0)
    user_data[user_id]['current_question'] = question_data['question']
    user_data[user_id]['current_correct_answer'] = question_data[
        'correct_answer']

    markup = types.ReplyKeyboardMarkup(resize_keyboard=True, row_width=3)
    options = question_data['options'][:]
    random.shuffle(options)
    for option in options:
        markup.add(types.KeyboardButton(option))

    bot.send_message(user_id, question_data['question'], reply_markup=markup)


def send_photo(chat_id, name):
    access_key = '20lPUgr5B52rAB39I7X0tQzLv5HLWQFIxdPul27-Fbs'
    random_page = random.randint(1, 3)

    if name == 'LV':
        search_query = 'Louis Vuitton'
    elif name == 'Porsche':
        search_query = 'Porsche'
    else:
        search_query = 'IPhone 14'
    url = f'https://api.unsplash.com/search/photos?page={random_page}&' \
          f'per_page=10&query={search_query}&' \
          f'orientation=landscape&client_id={access_key}'
    response = requests.get(url)

    if response.status_code == 200:
        data = json.loads(response.text)
        if data['results']:
            photo = random.choice(data['results'])
            photo_url = photo['urls']['regular']
            bot.send_photo(chat_id, photo_url)
        else:
            bot.send_message(chat_id, "No photos")
    else:
        bot.send_message(chat_id, "Error")


@bot.message_handler(commands=['start'])
def start_command(message):
    markup = types.ReplyKeyboardMarkup(resize_keyboard=True, row_width=2)
    for brand in brands:
        markup.add(types.KeyboardButton(brand))
    bot.send_message(message.chat.id, 'Choose a brand:', reply_markup=markup)


@bot.message_handler(content_types=['text'])
def handle_message(message):
    user_id = message.chat.id

    if user_id not in user_data or message.text in brands:
        user_data[user_id] = {
            'selected_brand': message.text,
            'score': 0,
            'questions_answered': 0,
            'remaining_questions': brands[message.text][:],
            'current_question': None,
            'current_correct_answer': None
        }
        random.shuffle(user_data[user_id]['remaining_questions'])
        ask_question(user_id)
    else:
        user_data[user_id]['questions_answered'] += 1

        if message.text == user_data[user_id]['current_correct_answer']:
            user_data[user_id]['score'] += 1
            response = "Correct!"
        else:
            response = f"Incorrect! The correct answer is " \
                       f"{user_data[user_id]['current_correct_answer']}"

        bot.send_message(user_id, response)

        send_photo(message.chat.id, user_data[user_id]['selected_brand'])

        if user_data[user_id]['questions_answered'] < 5:
            ask_question(user_id)
        else:
            response += f"\nYou answered {user_data[user_id]['score']}" \
                        f" out of 5 questions correctly."
            bot.send_message(user_id, response,
                             reply_markup=types.ReplyKeyboardRemove())
            del user_data[user_id]
            start_command(message)


bot.polling(none_stop=True)
