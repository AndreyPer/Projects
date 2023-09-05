import constants
import pygame
import random
import shared


class Bonus:
    """
    Represents a Bonus object that the snake can collect to gain points.
    """
    def __init__(self, apple, snake_body, obstacles):
        """
        Initializes a Bonus object with a random position that doesn't collide with the apple, snake, or obstacles.
        """
        while True:
            self.position = (
                random.randint(0, constants.GRID_WIDTH - 1),
                random.randint(0, constants.GRID_HEIGHT - 1)
            )
            if self.position != apple.position and self.position not in snake_body and \
                    self.position not in [obstacle.position for obstacle in obstacles]:
                if shared.ANTI_BONUS is not None:
                    if shared.ANTI_BONUS.position != self.position:
                        break
                break

    def draw(self, screen):
        """
        Draws the Bonus object on the screen.
        """
        x, y = self.position
        pygame.draw.rect(screen, constants.SKIN, (x * constants.GRID_SIZE, y * constants.GRID_SIZE,
                                                  constants.GRID_SIZE, constants.GRID_SIZE))


class Anti_Bonus:
    """
    Represents an Anti-Bonus object that the snake should avoid, as it may have negative effects.
    """
    def __init__(self, apple, snake_body, obstacles):
        """
        Initializes an Anti-Bonus object with a random position that doesn't collide with the apple, snake or obstacles.
        """
        while True:
            self.position = (
                random.randint(0, constants.GRID_WIDTH - 1),
                random.randint(0, constants.GRID_HEIGHT - 1)
            )
            if self.position != apple.position and self.position not in snake_body and \
                    self.position not in [obstacle.position for obstacle in obstacles]:
                if shared.BONUS is not None:
                    if shared.BONUS.position != self.position:
                        break
                break

    def draw(self, screen):
        """
        Draws the Anti-Bonus object on the screen.
        """
        x, y = self.position
        pygame.draw.rect(screen, constants.BLACK, (x * constants.GRID_SIZE, y * constants.GRID_SIZE,
                                                   constants.GRID_SIZE, constants.GRID_SIZE))


class SlowLandscape:
    """
    Represents the Slow Landscape effect, which reduces the game's speed when active.
    """
    def __init__(self):
        """
        Initializes the Slow Landscape effect with a predefined color.
        """
        self.color = constants.LIGHT_BLUE

    def draw(self, screen):
        """
        Draws the Slow Landscape effect's border on the screen.
        """
        wid = 6
        pygame.draw.rect(screen, self.color, (0, 0, constants.SCREEN_WIDTH, constants.SCREEN_HEIGHT), width=wid)

    def effect(self, game):
        """
        Applies the Slow Landscape effect on the game, reducing its speed.
        """
        game.fps -= 3
        if game.fps < 1:
            game.fps = 1


class FastLandscape:
    """
    Represents the Fast Landscape effect, which increases the game's speed when active.
    """
    def __init__(self):
        """
        Initializes the Fast Landscape effect with a predefined color.
        """
        self.color = constants.LIGHT_GREEN

    def draw(self, screen):
        """
        Draws the Fast Landscape effect's border on the screen.
        """
        wid = 6
        pygame.draw.rect(screen, self.color, (0, 0, constants.SCREEN_WIDTH, constants.SCREEN_HEIGHT), width=wid)

    def effect(self, game):
        """
        Applies the Fast Landscape effect on the game, increasing its speed.
        """
        game.fps += 3
