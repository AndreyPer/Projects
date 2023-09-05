import constants
import os
import pygame
from game import main_menu

if not os.path.exists(constants.RECORDS_FILE):
    with open(constants.RECORDS_FILE, "w") as f:
        f.write("0\n")


def main():
    """
    Set up the game window and start the main menu.
    """
    screen = pygame.display.set_mode((constants.SCREEN_WIDTH, constants.SCREEN_HEIGHT))
    pygame.display.set_caption("Snake Game")
    main_menu(screen)


if __name__ == '__main__':
    """
    Initialize pygame and start the main function.
    """
    pygame.init()
    main()
