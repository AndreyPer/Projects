import constants
import pygame
from level import Levels
import shared
from bonuses import SlowLandscape
from bonuses import FastLandscape


def draw(screen, snake, apple, level, fps):
    """
    Renders the game state, including the snake, apple, obstacles, bonuses, and game-related texts, on the screen.
    """
    screen.fill(constants.BACKGROUND)
    for obstacle in Levels().level_obstacles(level):
        obstacle.draw(screen)
    if shared.SLOW_TRUE:
        SlowLandscape().draw(screen)
    if shared.FAST_TRUE:
        FastLandscape().draw(screen)
    if shared.SNAKE_COLOR == constants.GOLD:
        snake_colors = constants.GREEN
    else:
        snake_colors = shared.SNAKE_COLOR

    for i, segment in enumerate(snake.body):
        color = snake_colors
        pygame.draw.rect(screen, color, (segment[0] * constants.GRID_SIZE, segment[1] * constants.GRID_SIZE,
                                         constants.GRID_SIZE, constants.GRID_SIZE))
        if shared.SNAKE_COLOR == constants.GOLD:
            pygame.draw.rect(screen, constants.RED, (segment[0] * constants.GRID_SIZE,
                                                     segment[1] * constants.GRID_SIZE + constants.GRID_SIZE // 3,
                                                     constants.GRID_SIZE, constants.GRID_SIZE // 3 + 2))

    pygame.draw.rect(screen, constants.RED, (apple.position[0] * constants.GRID_SIZE,
                                             apple.position[1] * constants.GRID_SIZE,
                                             constants.GRID_SIZE, constants.GRID_SIZE))

    if not (shared.BONUS is None):
        shared.BONUS.draw(screen)

    if not (shared.ANTI_BONUS is None):
        shared.ANTI_BONUS.draw(screen)

    score = len(snake.body) - 1
    font = pygame.font.Font(None, constants.FONT_SZ)
    score_text = font.render(f"Score: {score}", True, constants.YELLOW)
    screen.blit(score_text, (constants.X1, constants.Y))
    score_text = font.render(f"Speed: {fps}", True, constants.YELLOW)
    screen.blit(score_text, (constants.X2, constants.Y))
    pygame.display.flip()
