import sys
import pygame

import random
import bonuses
import constants
from drawing import draw
from food import Apple
from level import Levels
from records import display_records
from records import update_records
from snake import Snake
import shared
from level_editor import LevelEditor


def draw_menu(screen, selected_option):
    """
    Render the main menu with the given selected option.
    """
    screen.fill(constants.BACKGROUND)
    font = pygame.font.Font(None, constants.FONT_SZ)
    title_font = pygame.font.Font(None, constants.TITLE_SZ)
    pygame.display.set_caption("Snake Game")
    title_text = title_font.render("Gucci Snake", True, constants.BLACK)

    options = [
        "Level 1",
        "Level 2",
        "Level 3",
        "Level 4",
        "Level 5",
        "Custom Level",
        "Create Level",
        "Records",
        "Snake Color",
        "Exit"
    ]

    screen.blit(title_text, (constants.SCREEN_WIDTH // 2 - title_text.get_width() // 2, constants.TITLE_Y))

    for i, option in enumerate(options):
        height = 120
        color = constants.BLUE if i == selected_option else constants.BLACK
        option_text = font.render(option, True, color)
        screen.blit(option_text,
                    (constants.SCREEN_WIDTH // 2 - option_text.get_width() // 2, height + i * constants.TITLE_Y))

    pygame.display.flip()


def main_menu(screen):
    """
    Handle the main menu navigation and selection.
    """
    selected_option = 0
    rec = 7
    col = 8
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_DOWN:
                    selected_option = (selected_option + 1) % constants.OPTIONS_NUM
                elif event.key == pygame.K_UP:
                    selected_option = (selected_option - 1) % constants.OPTIONS_NUM
                elif event.key == pygame.K_RETURN:
                    if selected_option < constants.CUSTOM_NUM:
                        Game().run(selected_option + 1)
                    elif selected_option == constants.CUSTOM_NUM:
                        LevelEditor().run()
                    elif selected_option == rec:
                        display_records(screen, constants.RECORDS_FILE)
                    elif selected_option == col:
                        choose_snake_color(screen)
                    else:
                        pygame.quit()
                        sys.exit()

        draw_menu(screen, selected_option)
        pygame.time.wait(constants.WAIT_TIME)


def draw_color_selection(screen, colors, selected_color):
    """
    Render the color selection menu with the given selected color.
    """
    screen.fill(constants.BACKGROUND)
    title_font = pygame.font.Font(None, constants.TITLE_SZ)
    title_text = title_font.render("Choose Snake Color", True, constants.BLACK)

    screen.blit(title_text, (constants.SCREEN_WIDTH // 2 - title_text.get_width() // 2, constants.TITLE_Y))

    for i, color in enumerate(colors):
        y = 40
        x1 = 50
        x2 = 17
        x3 = 16
        height = 150
        width = 33
        if color == "Gucci":
            pygame.draw.rect(screen, constants.GREEN,
                             (constants.SCREEN_WIDTH // 2 - x1, height + i * constants.TITLE_Y, width, y))
            pygame.draw.rect(screen, constants.RED,
                             (constants.SCREEN_WIDTH // 2 - x2, height + i * constants.TITLE_Y, width, y))
            pygame.draw.rect(screen, constants.GREEN,
                             (constants.SCREEN_WIDTH // 2 + x3, height + i * constants.TITLE_Y, width + 1, y))
        else:
            pygame.draw.rect(screen, color, (constants.SCREEN_WIDTH // 2 - x1,
                                             height + i * constants.TITLE_Y, x1 * 2, y))

        if i == selected_color:
            pygame.draw.rect(screen, constants.PURPLE,
                             (constants.SCREEN_WIDTH // 2 - x1, height + i * constants.TITLE_Y, x1 * 2, y), 3)
        else:
            pygame.draw.rect(screen, constants.BLACK,
                             (constants.SCREEN_WIDTH // 2 - x1, height + i * constants.TITLE_Y, x1 * 2, y), 1)

    pygame.display.flip()


def choose_snake_color(screen):
    """
    Handle snake color selection and assignment.
    """
    pygame.display.set_caption("Choose color")

    colors = ["Gucci", constants.GREEN, constants.RED, constants.BLUE, constants.ORANGE, constants.TURQUOISE]
    selected_color = 0

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_DOWN:
                    selected_color = (selected_color + 1) % len(colors)
                elif event.key == pygame.K_UP:
                    selected_color = (selected_color - 1) % len(colors)
                elif event.key == pygame.K_RETURN:
                    if colors[selected_color] == "Gucci":
                        shared.SNAKE_COLOR = constants.GOLD
                    else:
                        shared.SNAKE_COLOR = colors[selected_color]
                    return
                elif event.key == pygame.K_ESCAPE:
                    return

        draw_color_selection(screen, colors, selected_color)
        pygame.time.wait(constants.WAIT_TIME)


def game_over(screen, score):
    """
    Display the game over screen with the given score.
    """
    font = pygame.font.Font(None, constants.FONT_SZ)
    title_font = pygame.font.Font(None, constants.TITLE_SZ)
    game_over_text = title_font.render("Game Over", True, constants.BLACK)
    score_text = font.render("Score: {}".format(score), True, constants.BLACK)
    instructions_text = font.render("Press ENTER to return to main menu or ESC to exit", True, constants.BLACK)

    while True:
        screen.fill(constants.BACKGROUND)
        screen.blit(game_over_text, (constants.SCREEN_WIDTH // 2 - game_over_text.get_width() // 2,
                                     constants.TITLE_Y * 2))
        screen.blit(score_text, (constants.SCREEN_WIDTH // 2 - score_text.get_width() // 2,
                                 constants.TITLE_Y * 4))
        screen.blit(instructions_text, (constants.SCREEN_WIDTH // 2 - instructions_text.get_width() // 2,
                                        constants.TITLE_Y * 6))
        pygame.display.flip()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_RETURN:
                    return
                elif event.key == pygame.K_ESCAPE:
                    pygame.quit()
                    sys.exit()

        pygame.time.wait(constants.WAIT_TIME)


class Game:
    """
    The main Game class for the Snake game, handling game loop, movement, and collisions.
    """
    def __init__(self):
        """
        Initialize the game state variables.
        """
        self.fps = 10
        self.food_cnt = 0
        self.elapsed_time = 0
        self.bonus_spawn_timer = 0
        self.anti_bonus_spawn_timer = 0
        self.anti_bonus_timer = 0
        self.bonus_timer = 0
        self.slow_landscape_spawn_timer = 0
        self.slow_landscape_timer = 0
        self.fast_landscape_spawn_timer = 0
        self.fast_landscape_timer = 0

    def move(self, snake, screen):
        """
        Handle user input for snake movement and other controls.
        """
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_UP:
                    snake.change_direction((0, -1))
                elif event.key == pygame.K_DOWN:
                    snake.change_direction((0, 1))
                elif event.key == pygame.K_LEFT:
                    snake.change_direction((-1, 0))
                elif event.key == pygame.K_RIGHT:
                    snake.change_direction((1, 0))
                elif event.key == pygame.K_q:
                    snake.grow()
                elif event.key == pygame.K_w:
                    snake.shrink()
                elif event.key == pygame.K_p:
                    self.pause(screen)
                elif event.key == pygame.K_ESCAPE:
                    return True
        snake.move()
        snake.direction_changed = False
        return False

    def check_death(self, snake, level):
        """
        Check if the snake has collided with the boundary, itself, or an obstacle.
        """
        if (
                snake.body[0][0] < 0 or snake.body[0][0] >= constants.GRID_WIDTH or
                snake.body[0][1] < 0 or snake.body[0][1] >= constants.GRID_HEIGHT or
                snake.body[0] in snake.body[1:] or
                any([obstacle.position == snake.body[0] for obstacle in Levels().level_obstacles(level)])
        ):
            return True
        else:
            return False

    def handle_collisions(self, snake, apple, level):
        """
        Handle collisions between the snake and apple or bonuses.
        """
        if not (shared.ANTI_BONUS is None):
            if snake.body[0] == shared.ANTI_BONUS.position:
                return True

        if not (shared.BONUS is None):
            if snake.body[0] == shared.BONUS.position:
                self.bonus_spawn_timer = 0
                self.bonus_timer = 0
                snake.grow(constants.APPLES_NUM)
                shared.BONUS = None
        if snake.body[0] == apple.position:
            self.food_cnt += 1
            snake.grow()
            apple.respawn(snake.body, level)
        return False

    def spawn_bonuses(self, apple, snake, level):
        """
        Spawn bonuses and handle bonus timers.
        """
        if self.bonus_timer >= constants.SEC5:
            self.bonus_spawn_timer = 0
            self.bonus_timer = 0
            shared.BONUS = None

        if self.anti_bonus_timer >= constants.SEC5:
            self.anti_bonus_spawn_timer = 0
            self.anti_bonus_timer = 0
            shared.ANTI_BONUS = None

        if self.slow_landscape_timer >= constants.SEC5:
            self.fps += 3
            self.slow_landscape_spawn_timer = 0
            self.slow_landscape_timer = 0
            shared.SLOW_TRUE = 0
            shared.SLOW_LANDSCAPE = None

        if self.fast_landscape_timer >= constants.SEC5:
            self.fps -= 3
            self.fast_landscape_spawn_timer = 0
            self.fast_landscape_timer = 0
            shared.FAST_TRUE = 0
            shared.FAST_LANDSCAPE = None

        if self.bonus_spawn_timer > constants.SEC15:
            self.bonus_spawn_timer = 0

        if self.anti_bonus_spawn_timer > constants.SEC15:
            self.anti_bonus_spawn_timer = 0

        if self.slow_landscape_spawn_timer > constants.SEC20:
            self.slow_landscape_spawn_timer = 0

        if self.fast_landscape_spawn_timer > constants.SEC15:
            self.fast_landscape_spawn_timer = 0

        # Spawn a bonuses.BONUS at a random interval between 5 and 15 seconds
        if shared.BONUS is None and constants.SEC5 <= self.bonus_spawn_timer <= constants.SEC15 and \
                self.food_cnt >= constants.APPLES_NUM:
            if random.random() < constants.RAND_IND:
                shared.BONUS = bonuses.Bonus(apple, snake.body, Levels().level_obstacles(level))
                self.bonus_timer = 0
                self.food_cnt = 0
                self.bonus_spawn_timer = 0

        if shared.ANTI_BONUS is None and constants.SEC5 <= self.anti_bonus_spawn_timer <= constants.SEC15:
            if random.random() < constants.RAND_IND:
                shared.ANTI_BONUS = bonuses.Anti_Bonus(apple, snake.body, Levels().level_obstacles(level))
                self.anti_bonus_timer = 0
                self.anti_bonus_spawn_timer = 0

        # Spawn a SlowLandscape at a random interval between 10 and 20 seconds
        if shared.SLOW_TRUE == 0 and shared.FAST_TRUE == 0 and \
                constants.SEC10 <= self.slow_landscape_spawn_timer <= constants.SEC20:
            if random.random() < constants.RAND_IND and shared.FAST_LANDSCAPE is None:
                shared.SLOW_LANDSCAPE = bonuses.SlowLandscape()
                shared.SLOW_TRUE = 1
                shared.FAST_TRUE = 0
                self.slow_landscape_timer = 0
                self.slow_landscape_spawn_timer = 0

        if shared.SLOW_TRUE == 0 and shared.FAST_TRUE == 0 and \
                constants.SEC5 <= self.fast_landscape_spawn_timer <= constants.SEC15:
            if random.random() < constants.RAND_IND and shared.SLOW_LANDSCAPE is None:
                shared.FAST_LANDSCAPE = bonuses.FastLandscape()
                shared.FAST_TRUE = 1
                shared.SLOW_TRUE = 0
                self.fast_landscape_timer = 0
                self.fast_landscape_spawn_timer = 0

    def pause(self, screen):
        """
        Pause the game and display a pause message.
        """
        pause_font = pygame.font.Font(None, constants.FONT_SZ)
        pause_text = pause_font.render("Game Paused. Press 'P' to continue.", True, constants.WHITE)
        pause_rect = pause_text.get_rect(center=(constants.SCREEN_WIDTH // 2, constants.SCREEN_HEIGHT // 2))
        screen.blit(pause_text, pause_rect)
        pygame.display.flip()

        while True:
            event = pygame.event.wait()
            if event.type == pygame.KEYDOWN and event.key == pygame.K_p:
                break

    def run(self, level):
        """
        Run the game loop for the given level.
        """
        screen = pygame.display.set_mode((constants.SCREEN_WIDTH, constants.SCREEN_HEIGHT))
        if level < constants.CUSTOM_NUM:
            pygame.display.set_caption(f"Level {level}")
        else:
            pygame.display.set_caption("Custom Level")

        snake = Snake()
        apple = Apple(snake.body, level)
        clock = pygame.time.Clock()

        while True:
            if self.move(snake, screen):
                break

            if self.handle_collisions(snake, apple, level):
                break

            if self.check_death(snake, level):
                break

            draw(screen, snake, apple, level, self.fps)
            clock.tick(self.fps)
            self.elapsed_time += clock.get_time()
            if self.elapsed_time >= constants.SEC10:
                self.fps += 1
                self.elapsed_time = 0

            self.bonus_spawn_timer += clock.get_time()
            self.anti_bonus_spawn_timer += clock.get_time()
            self.slow_landscape_spawn_timer += clock.get_time()
            self.fast_landscape_spawn_timer += clock.get_time()

            if not (shared.SLOW_LANDSCAPE is None):
                shared.SLOW_LANDSCAPE.effect(self)
                shared.SLOW_LANDSCAPE = None
            if not (shared.FAST_LANDSCAPE is None):
                shared.FAST_LANDSCAPE.effect(self)
                shared.FAST_LANDSCAPE = None

            if not (shared.BONUS is None):
                self.bonus_timer += clock.get_time()

            if not (shared.ANTI_BONUS is None):
                self.anti_bonus_timer += clock.get_time()

            if shared.SLOW_TRUE:
                self.slow_landscape_timer += clock.get_time()

            if shared.FAST_TRUE:
                self.fast_landscape_timer += clock.get_time()

            self.spawn_bonuses(apple, snake, level)

        score = len(snake.body) - 1
        update_records(score, constants.RECORDS_FILE)
        shared.SLOW_LANDSCAPE = None
        shared.FAST_LANDSCAPE = None
        shared.FAST_TRUE = 0
        shared.SLOW_TRUE = 0
        self.slow_landscape_spawn_timer = 0
        self.fast_landscape_spawn_timer = 0
        self.slow_landscape_timer = 0
        self.fast_landscape_timer = 0
        shared.BONUS = None
        shared.ANTI_BONUS = None
        game_over(screen, score)
