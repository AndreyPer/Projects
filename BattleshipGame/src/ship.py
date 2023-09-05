import math
from geometry import Geometry
import globals
import pygame
from constants import *
import sys


class Ship:
    def output_error(self, code):
        messages = {
            1: "Wrong cell. Please, try again!",
            2: "This cell is occupied. Please, try again!",
            3: "You have clicked the same cells. Please, try again!",
            4: "You have clicked next to the existing ship. Please, try again!",
            5: "The ship should consist of adjacent cells. Please, try again!"
        }
        Geometry().building(globals.screen_cond[self.pl], 0, 0)
        Geometry().print_warning(messages[code])
        decks = {4: ' four-deck', 3: ' three-deck', 2: ' two-deck',
                 1: ' one-deck'}
        pr = ("Player" + str(
            self.pl + 1) + (", please, click on positions of your" +
                            decks[self.size] + " ship"))
        Geometry().print_text(pr)
        Geometry().update()
        return False

    def check_build(self):
        if (len(set(self.let)) == 1 and len(set(self.num)) < self.size) or (
                len(set(self.num)) == 1 and len(set(self.let)) < self.size):
            return self.output_error(3)  # identical cells among the entered

        check_the_ship = 0
        if len(set(self.let)) == 1:
            for u in range(self.size - 1):
                if self.num[u] + 1 != self.num[u + 1]:
                    check_the_ship = 1

        if len(set(self.num)) == 1:
            for u in range(self.size - 1):
                if self.let[u] + 1 != self.let[u + 1]:
                    check_the_ship = 1

        if check_the_ship or len(set(self.let)) != 1 and \
                len(set(self.num)) != 1:  # There is a gap in the ship
            return self.output_error(5)
        return True

    def processing(self):
        delta = 26
        angle = 5
        length = 10.8
        dist = 15
        x1 = left_margin
        y1 = upper_margin + block_size * length
        x2 = block_size * (delta - 1)
        y2 = block_size * 2
        Geometry().rectangle((0, 0, 0), x1, y1, x2, y2, 0, angle)
        self.let.append(globals.x)
        self.num.append(globals.y)
        center = left_margin + (globals.x - 1) * block_size + (
                self.pl * dist * block_size + delta)
        rad = upper_margin + (globals.y - 1) * block_size + delta
        Geometry().draw_circle(center, rad)
        Geometry().update()

    def sorting(self):
        self.let.sort()
        self.num.sort()

    def check_input(self):
        decks = {4: ' four-deck', 3: ' three-deck', 2: ' two-deck',
                 1: ' one-deck'}
        pr = "Player{}, please click on positions of your{} ship".format(
            self.pl + 1, decks[self.size])
        Geometry().print_text(pr)
        while len(self.num) < self.size:
            event = self.get_event()
            if event is not None:
                self.handle_event()
            pygame.time.Clock().tick(FPS)
        self.sorting()
        return self.check_build()

    def get_event(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.MOUSEBUTTONDOWN:
                return event
        return None

    def handle_event(self):
        dist = 15
        cell_num = 10
        mouse_pos = pygame.mouse.get_pos()
        globals.x = math.ceil((mouse_pos[0] - (
                    left_margin + block_size * dist * self.pl)) / block_size)
        globals.y = math.ceil((mouse_pos[1] - upper_margin) / block_size)
        if globals.x > cell_num or globals.y > cell_num or mouse_pos[
            0] < left_margin + block_size * dist * abs(self.pl) or \
                mouse_pos[1] < upper_margin:
            return self.output_error(1)
        elif globals.condition[self.pl][globals.x][globals.y] != 0:
            return self.output_error(2)
        if self.has_neighbors():
            return self.output_error(4)
        self.processing()

    def has_neighbors(self):
        checker = [-1, 0, 1]
        for j in range(3):
            for e in range(3):
                if globals.condition[self.pl][globals.x +
                                              checker[j]][globals.y +
                                                          checker[e]] != 0:
                    return True
        return False

    def draw(self):
        dist = 15
        delta = 6
        ship_col = (31, 174, 233)
        for globals.y in range(self.size):
            cur_x = self.let[globals.y]
            cur_y = self.num[globals.y]
            if globals.condition[self.pl][cur_x][cur_y] != 3:
                globals.condition[self.pl][cur_x][cur_y] = 1
        Geometry().building(globals.screen_cond[self.pl], 0, 0)
        x1 = left_margin + (self.let[0] - 1) * block_size + (
                self.pl * dist * block_size - 2)
        y1 = upper_margin + (self.num[0] - 1) * block_size - 2
        x2 = (self.let[self.size - 1] - self.let[0] + 1) * block_size + delta
        y2 = (self.num[self.size - 1] - self.num[0] + 1) * block_size + delta
        Geometry().rectangle(ship_col, x1, y1, x2, y2, delta + 2, delta)

        Geometry().draw_black()
        Geometry().update()
        globals.screen_cond[self.pl] = globals.screen.copy()
        globals.counter[self.size - 1][self.pl] += 1

    def __init__(self, size, pl):
        self.size = size
        self.pl = pl
        self.cnt = size
        self.let = []
        self.num = []

        decks = {4: ' four-deck', 3: ' three-deck', 2: ' two-deck',
                 1: ' one-deck'}
        globals.screen_cond[pl] = globals.screen.copy()
        pr = "Player" + str(pl + 1) + ", please, click on positions of your" + (
                decks[size] + " ship")
        Geometry().print_text(pr)

        while not self.check_input():
            self.let = []
            self.num = []

        self.draw()
