import sys
import globals
import pygame
from constants import *


class Geometry:
    def circle(self, col, x, y, rad, fill):
        pygame.draw.circle(globals.screen, col, (x, y), rad, fill)

    def rectangle(self, col, x, y, length, width, fill, angle):
        pygame.draw.rect(globals.screen, col, pygame.Rect(x, y, length, width),
                         fill, angle)

    def line(self, col, x, y, length, width, fill):
        pygame.draw.line(globals.screen, col, (x, y), (length, width), fill)

    def building(self, f, s, t):
        globals.screen.blit(f, (s, t))

    def print_text(self, s1):
        width = 25
        height = 4
        col = (142, 173, 97)
        angle = 5
        building_size = 13
        font_size = 50
        x1 = left_margin
        y1 = upper_margin + block_size * width / 2
        x2 = block_size * width
        y2 = block_size * height
        Geometry().rectangle((0, 0, 0), x1, y1, x2, y2, 0, angle)
        text_surface = pygame.font.SysFont('notosans', font_size).render(s1,
                                                                         True,
                                                                         col)
        Geometry().building(text_surface, left_margin,
                            upper_margin + block_size * building_size)
        Geometry().update()

    def print_warning(self, s1):
        width = 25
        height = 1
        length = 10.8
        col = (142, 173, 97)
        angle = 5
        building_size = 11.5
        font_size = 50
        x1 = left_margin
        y1 = upper_margin + block_size * length
        x2 = block_size * width
        y2 = block_size * height
        Geometry().rectangle((0, 0, 0), x1, y1, x2, y2, 0, angle)
        text_surface = pygame.font.SysFont('notosans', font_size).render(
            s1, True, col)
        globals.screen.blit(text_surface, (left_margin, upper_margin + (
                block_size * building_size)))
        Geometry().update()

    def draw_black(self):
        width = 25
        height = 4
        length = 10.5
        angle = 5
        x1 = left_margin
        y1 = upper_margin + block_size * length
        x2 = block_size * width
        y2 = block_size * height
        Geometry().rectangle((0, 0, 0), x1, y1, x2, y2, 0, angle)

    def draw_circle(self, pos_x, pos_y):
        grey_color = (127, 118, 121)
        circle_radius = 20
        circle_width = 4
        Geometry().circle(grey_color, pos_x, pos_y, circle_radius, circle_width)

    def update(self):
        pygame.display.update()

    def fill(self, r, g, b):
        globals.screen.fill((r, g, b))

    def first_announcement(self, s):
        font_size = 80
        col = (142, 173, 97)
        length = 7.5
        Geometry().fill(0, 0, 0)
        my_announcement_font = pygame.font.SysFont('notosans', font_size)
        text_surface = my_announcement_font.render(s, True, col)
        Geometry().building(text_surface, left_margin, upper_margin + (
                block_size * length))
        Geometry().update()

    def second_announcement(self, s):
        font_size = 80
        col = (142, 173, 97)
        length = 6
        my_announcement_font = pygame.font.SysFont('notosans', font_size)
        text_surface = my_announcement_font.render(s, True, col)
        Geometry().building(text_surface, left_margin, upper_margin + (
                block_size * length))
        Geometry().update()

    def third_announcement(self, s):
        font_size = 80
        col = (142, 173, 97)
        length = 8
        my_announcement_font = pygame.font.SysFont('notosans', font_size)
        text_surface = my_announcement_font.render(s, True, col)
        Geometry().building(text_surface, left_margin, upper_margin + (
                block_size * length))
        Geometry().update()

    def click_wait(self):
        clock = pygame.time.Clock()
        click = False
        while not click:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    sys.exit()
                if event.type == pygame.MOUSEBUTTONDOWN:
                    click = True
            clock.tick(FPS)
