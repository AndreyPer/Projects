import constants


class Snake:
    """
    Class representing the snake in the game.
    """
    def __init__(self):
        """
        Initialize the snake with a starting position and direction.
        """
        self.body = [(constants.GRID_WIDTH // 2, constants.GRID_HEIGHT // 2)]
        self.direction = (0, 1)
        self.growing = False
        self.grow_by = 1
        self.grow_timer = 0
        self.direction_changed = False

    def move(self):
        """
        Move the snake in its current direction, handling growth and wrapping around the screen.
        """
        new_head = (self.body[0][0] + self.direction[0], self.body[0][1] + self.direction[1])
        new_head = self.wrap_position(new_head)

        if self.growing and self.grow_timer < self.grow_by:
            self.body.insert(0, new_head)
            self.grow_timer += 1
        else:
            if self.growing:
                self.growing = False
                self.grow_timer = 0
                self.grow_by = 1
            self.body.pop()
            self.body.insert(0, new_head)

    def grow(self, segments=1):
        """
        Grow the snake by a specified number of segments.
        """
        self.growing = True
        self.grow_by = segments

    def wrap_position(self, position):
        """
        Wrap the position of the snake around the screen if it goes off the edge.
        """
        x, y = position
        return x % constants.GRID_WIDTH, y % constants.GRID_HEIGHT

    def change_direction(self, new_direction):
        """
        Change the direction of the snake, ensuring it does not reverse its direction.
        """
        if self.direction_changed:
            return

        if self.direction[0] == -new_direction[0] and self.direction[1] == -new_direction[1]:
            return

        self.direction = new_direction
        self.direction_changed = True

    def shrink(self):
        """
        Shrink the snake by one segment, ensuring it doesn't shrink below the minimum length.
        """
        min_length = 1
        if len(self.body) > min_length:
            self.body.pop()
