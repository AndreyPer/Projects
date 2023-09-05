import constants
import pygame
import sys


def update_records(score, RECORDS_FILE):
    """
    Update the record scores with the given score and write it to the RECORDS_FILE.
    """
    with open(RECORDS_FILE, "r") as f:
        records = [int(line.strip()) for line in f.readlines()]
    records.append(score)
    records.sort(reverse=True)
    with open(RECORDS_FILE, "w") as f:
        for record in records[:10]:
            f.write("{}\n".format(record))


def display_records(screen, RECORDS_FILE):
    """
    Display the record scores on the screen, reading from the RECORDS_FILE.
    """
    with open(RECORDS_FILE, "r") as f:
        records = [int(line.strip()) for line in f.readlines()]

    draw_records(screen, records)

    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            elif event.type == pygame.KEYDOWN:
                if event.key in (pygame.K_ESCAPE, pygame.K_RETURN):
                    return

        pygame.time.wait(constants.WAIT_TIME)


def draw_records(screen, records):
    """
    Draw the record scores on the screen.
    """
    font = pygame.font.Font(None, constants.FONT_SZ)
    title_font = pygame.font.Font(None, constants.TITLE_SZ)
    pygame.display.set_caption("Records")

    title_text = title_font.render("Records", True, constants.BLACK)

    screen.fill(constants.BACKGROUND)
    screen.blit(title_text, (constants.SCREEN_WIDTH // 2 - title_text.get_width() // 2, constants.TITLE_Y))

    for i, record in enumerate(records):
        text = font.render(f"{i + 1}. {record}", True, constants.BLACK)
        screen.blit(text, (constants.SCREEN_WIDTH // 2 - text.get_width() // 2,
                           constants.TITLE_Y * 4 + i * constants.TITLE_Y))

    pygame.display.flip()
