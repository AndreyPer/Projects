import math
import sys
from geometry import Geometry
from ship import Ship
from field import Field
import globals
from constants import *
import pygame


class Player:
    def clear_all(self):
        globals.condition = [[[0 for i in range(12)] for j in range(12)],
                             [[0 for i in range(12)] for j in range(12)]]
        globals.change_pos = [0, 0, 0, 0]
        globals.shoot = 0
        globals.hits = [0, 0]
        globals.screen = pygame.display.set_mode(globals.constants.size)
        globals.placement = globals.screen.copy()
        globals.screen_cond = [globals.screen.copy(), globals.screen.copy()]
        globals.counter = [[0, 0], [0, 0], [0, 0], [0, 0]]
        globals.accuracy = 0
        globals.all_ships = [[], []]
        globals.x = 0
        globals.y = 0

    def print_text(self, s1):
        height = 11
        width = 20
        angle = 5
        font_size = 50
        col = (142, 173, 97)
        block_num = 4
        length = 12
        x1 = left_margin
        y1 = upper_margin + block_size * height
        x2 = block_size * width
        y2 = block_size * block_num
        Geometry().rectangle((0, 0, 0), x1, y1, x2, y2, 0, angle)
        text_surface = pygame.font.SysFont('notosans', font_size).render(
            s1, True, col)
        Geometry().building(text_surface, left_margin, upper_margin + (
                block_size * length))
        Geometry().update()

    def check_death(self, player, turn):
        for ship in globals.all_ships[player]:
            if globals.x in ship.let and globals.y in ship.num:
                self.draw_death(player, ship, turn)

    def draw_death(self, player, ship, turn):
        dist = 15
        angle = 5
        death_col = (128, 0, 255)
        if globals.x in ship.let and globals.y in ship.num:
            ship.cnt -= 1 * turn
            if ship.cnt == 0:
                for k in range(len(ship.let)):
                    self.draw_misses(player, k, ship)
                for t in range(len(ship.let)):
                    globals.condition[abs(player)][ship.let[t]][ship.num[t]] = 3
                x1 = left_margin + (ship.let[0] - 1) * block_size + (
                        player * dist * block_size + 1)
                y1 = upper_margin + (ship.num[0] - 1) * block_size + 1
                x2 = (ship.let[ship.size - 1] - ship.let[0] + 1) * block_size
                y2 = (ship.num[ship.size - 1] - ship.num[0] + 1) * block_size
                Geometry().rectangle(death_col, x1, y1, x2, y2, 0, angle + 1)

    def draw_misses(self, player, k, ship):
        dist = 15
        angle = 5
        delta_1 = 16
        delta_2 = 9
        cell_num = 10
        checker = [-1, 0, 1]
        miss_col = (255, 228, 225)
        for n in range(3):
            for m in range(3):
                if cell_num >= ship.let[k] + checker[n] > 0 and \
                        cell_num >= ship.num[k] + checker[m] > 0:
                    nei_x = ship.let[k] + checker[n]
                    nei_y = ship.num[k] + checker[m]
                    globals.condition[abs(player)][nei_x][nei_y] = 2
                    x1 = left_margin + (
                            ship.let[k] + checker[n] - 1) * block_size + (
                                 dist * block_size * player + delta_2)
                    y1 = (upper_margin + (
                            (ship.num[k] + checker[m] - 1) * block_size +
                            delta_2))
                    x2 = block_size - delta_1
                    y2 = block_size - delta_1
                    Geometry().rectangle(miss_col, x1, y1, x2, y2, 0, angle)

    def start_game(self, pl):
        Geometry().building(globals.placement, 0, 0)
        for i in range(4, 0, -1):
            for j in range(5 - i):
                globals.all_ships[pl].append(Ship(i, pl))
        globals.screen_cond[pl] = globals.screen.copy()

    def turn_start(self, pl):
        hits_num = 20
        if globals.hits[0] == hits_num:
            return 0
        if globals.hits[1] == hits_num:
            return 0
        Geometry().first_announcement("Player" + str(pl + 1) + (
            ", please, tap on screen to make a move"))
        if globals.accuracy != 0:
            if globals.accuracy == 2:
                Geometry().second_announcement("Wrong cell. Please, try again!")
            elif globals.accuracy == 3:
                Geometry().second_announcement(
                    "You have already shot this cell. Try another one!")
            elif globals.shoot == 2:
                Geometry().second_announcement("You missed!")
            elif globals.shoot == 3:
                Geometry().second_announcement("You hit!")
        Geometry().update()
        Geometry().click_wait()
        Geometry().building(globals.placement, 0, 0)
        globals.screen_cond[pl] = globals.placement
        for ship in globals.all_ships[pl]:
            Ship.draw(ship)
        Geometry().print_text("Player" + str(pl + 1) + (
            ", please, click the coordinate to shoot at"))
        Geometry().update()
        Player().turn_main(pl)
        return 0

    def draw_cond(self, pl, col):
        dist = 15
        angle = 5
        delta_1 = 16
        delta_2 = 9
        x1 = left_margin + (globals.change_pos[pl * 2] - 1) * block_size + (
                dist * block_size * abs(pl - 1) + delta_2)
        y1 = upper_margin + (
                (globals.change_pos[pl * 2 + 1] - 1) * block_size + delta_2)
        x2 = block_size - delta_1
        y2 = block_size - delta_1
        Geometry().rectangle(col, x1, y1, x2, y2, 0, angle)

    def cond_0(self, pl):
        miss_col = (255, 228, 225)
        globals.accuracy = 1
        globals.shoot = 2
        globals.condition[abs(pl - 1)][globals.x][globals.y] = 2
        self.draw_cond(pl, miss_col)
        Geometry().draw_black()
        Geometry().update()
        globals.placement = globals.screen.copy()

    def cond_1(self, pl):
        hit_col = (0, 0, 0)
        globals.accuracy = 1
        globals.shoot = 3
        globals.hits[pl] += 1
        globals.condition[abs(pl - 1)][globals.x][globals.y] = 3
        self.draw_cond(pl, hit_col)
        Player().check_death(abs(pl - 1), 1)
        Geometry().draw_black()
        globals.placement = globals.screen.copy()
        Player().turn_start(pl)
        return 0

    def check_all(self, pl):
        if globals.condition[abs(pl - 1)][globals.x][globals.y] == 0:
            self.cond_0(pl)
            return 0

        elif globals.condition[abs(pl - 1)][globals.x][globals.y] == 1:
            self.cond_1(pl)
            return 0

        elif globals.condition[abs(pl - 1)][globals.x][globals.y] == 2 or \
                globals.condition[abs(pl - 1)][globals.x][globals.y] == 3:
            globals.accuracy = 3
            Player().turn_start(pl)
            return 0

    def turn_main(self, pl):
        dist = 15
        cell_num = 10
        clock = pygame.time.Clock()
        click = False
        while not click:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
                elif event.type == pygame.MOUSEBUTTONDOWN:
                    mouse_pos = pygame.mouse.get_pos()
                    globals.x = math.ceil((mouse_pos[0] - (
                                left_margin +
                                block_size * dist * abs(pl - 1))) / block_size)
                    globals.y = math.ceil(
                        (mouse_pos[1] - upper_margin) / block_size)
                    click = True
            clock.tick(FPS)
        if globals.x > cell_num or globals.y > cell_num or mouse_pos[
            0] < left_margin + block_size * dist * \
                abs(pl - 1) or mouse_pos[1] < upper_margin:
            globals.accuracy = 2
            Geometry().building(globals.screen_cond[pl], 0, 0)
            Player.turn_start(self, pl)
            return 0
        self.update_globals(pl)
        self.check_all(pl)
        Geometry().update()
        globals.placement = globals.screen.copy()

    def update_globals(self, pl):
        globals.change_pos[pl * 2] = globals.x
        globals.change_pos[pl * 2 + 1] = globals.y
        Geometry().building(globals.placement, 0, 0)
        Geometry().update()

    def play(self):
        hits_num = 20
        pygame.font.init()
        pygame.init()
        pygame.display.set_caption("Ocean Boy")
        field = Field()
        field.two_fields()
        player1 = Player()
        player2 = Player()
        player1.start_game(0)
        player2.start_game(1)
        while globals.hits[0] != hits_num and globals.hits[1] != hits_num:
            player1.turn_start(0)
            if globals.hits[0] == hits_num:
                break
            player2.turn_start(1)
        Geometry().fill(0, 0, 0)
        if globals.hits[0] == hits_num:
            Geometry().second_announcement("Player1 won!")
        else:
            Geometry().second_announcement("Player2 won!")
        Geometry().third_announcement("Please, click anywhere to play again")
        Geometry().update()
        Geometry().click_wait()
        self.clear_all()
        self.play()
        return 0
