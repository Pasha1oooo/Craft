#include <ctype.h>
#include <notcurses/notcurses.h>

struct text_pos
{
	int x;
	int y;
};

enum {
	X_INDENT = 2,
	Y_INDENT = 1,
	LETTERS_AMOUNT = 26,
	DIGITS_AMOUNT = 10,
	SPECIAL_AMOUNT = 1,
	CH_X_SIZE = 5,
	CH_Y_SIZE = 5
};

char LETTERS[LETTERS_AMOUNT][CH_Y_SIZE][CH_X_SIZE + 1] = {
	{" ### ",
	 "#   #",
	 "#####",
	 "#   #",
	 "#   #"},

	{"#### ",
	 "#   #",
	 "#### ",
	 "#   #",
	 "#### "},

	{" ### ",
	 "#   #",
	 "#    ",
	 "#   #",
	 " ### "},

	{"#### ",
	 "#   #",
	 "#   #",
	 "#   #",
	 "#### "},

	{"#####",
	 "#    ",
	 "#### ",
	 "#    ",
	 "#####"},

	{"#####",
	 "#    ",
	 "#### ",
	 "#    ",
	 "#    "},

	{" ####",
	 "#    ",
	 "#  ##",
	 "#   #",
	 " ### "},

	{"#   #",
	 "#   #",
	 "#####",
	 "#   #",
	 "#   #"},

	{" ### ",
	 "  #  ",
	 "  #  ",
	 "  #  ",
	 " ### "},

	{"    #",
	 "    #",
	 "    #",
	 "#   #",
	 " ### "},

	{"#   #",
	 "#  # ",
	 "###  ",
	 "#  # ",
	 "#   #"},

	{"#    ",
	 "#    ",
	 "#    ",
	 "#    ",
	 "#####"},

	{"## ##",
	 "# # #",
	 "# # #",
	 "#   #",
	 "#   #"},

	{"#   #",
	 "##  #",
	 "# # #",
	 "#  ##",
	 "#   #"},

	{" ### ",
	 "#   #",
	 "#   #",
	 "#   #",
	 " ### "},

	{"#### ",
	 "#   #",
	 "#### ",
	 "#    ",
	 "#    "},

	{" ### ",
	 "#   #",
	 "#   #",
	 "#  ##",
	 " ####"},

	{"#### ",
	 "#   #",
	 "#### ",
	 "#  # ",
	 "#   #"},

	{" ####",
	 "#    ",
	 " ### ",
	 "    #",
	 "#### "},

	{"#####",
	 "  #  ",
	 "  #  ",
	 "  #  ",
	 "  #  "},

	{"#   #",
	 "#   #",
	 "#   #",
	 "#   #",
	 " ### "},

	{"#   #",
	 "#   #",
	 "#   #",
	 " # # ",
	 "  #  "},

	{"#   #",
	 "#   #",
	 "# # #",
	 "# # #",
	 " # # "},

	{"#   #",
	 " # # ",
	 "  #  ",
	 " # # ",
	 "#   #"},

	{"#   #",
	 " # # ",
	 "  #  ",
	 "  #  ",
	 "  #  "},

	{"#####",
	 "   # ",
	 "  #  ",
	 " #   ",
	 "#####"}
};

char DIGITS[DIGITS_AMOUNT][CH_Y_SIZE][CH_X_SIZE + 1] = {
	{" ### ",
	 "#  ##",
	 "# # #",
	 "##  #",
	 " ### "},

	{"   # ",
	 "  ## ",
	 " # # ",
	 "   # ",
	 "   # "},

	{"#### ",
	 "    #",
	 " ### ",
	 "#    ",
	 "#####"},

	{"#### ",
	 "    #",
	 " ### ",
	 "    #",
	 "#### "},

	{"#   #",
	 "#   #",
	 " ####",
	 "    #",
	 "    #"},

	{"#####",
	 "#    ",
	 " ### ",
	 "    #",
	 "#### "},

	{" ####",
	 "#    ",
	 "#### ",
	 "#   #",
	 " ### "},

	{"#####",
	 "    #",
	 "   # ",
	 "  #  ",
	 "  #  "},

	{" ### ",
	 "#   #",
	 " ### ",
	 "#   #",
	 " ### "},

	{" ### ",
	 "#   #",
	 " ####",
	 "    #",
	 "####"},
};

char SPECIAL[SPECIAL_AMOUNT][CH_Y_SIZE][CH_X_SIZE + 1] = {
	{" #   ",
	 "     ",
	 "     ",
	 "     ",
	 " #   "},
};

void putchar2menu(struct ncplane *child_plane, char ch, struct text_pos *pos)
{
	ch = toupper(ch);

	if (ch == ' ')
		return;

	if (ch == '\n') {
		pos->x = -CH_X_SIZE + 1;
		pos->y += Y_INDENT + CH_Y_SIZE;
	}

	if (ch == ':') {
		for (int i = pos->y; i < pos->y + CH_Y_SIZE; i++)
			ncplane_printf_yx(child_plane, i, pos->x, "%s",
			                  SPECIAL[0][i - pos->y]);
	}

	if (isalpha(ch)) {
		int letter = ch - 'A';

		for (int i = pos->y; i < pos->y + CH_Y_SIZE; i++)
			ncplane_printf_yx(child_plane, i, pos->x, "%s",
			                  LETTERS[letter][i - pos->y]);
	}

	if (isdigit(ch)) {
		int digit = ch - '0';

		for (int i = pos->y; i < pos->y + CH_Y_SIZE; i++)
			ncplane_printf_yx(child_plane, i, pos->x, "%s",
			                  DIGITS[digit][i - pos->y]);
	}
}

void print2menu(struct ncplane *child_plane, char *str)
{
	struct text_pos pos = {.x = X_INDENT, .y = Y_INDENT};
	char ch;

	for (int i = 0;; i++) {
		ch = str[i];

		if (ch == '\0')
			break;

		putchar2menu(child_plane, ch, &pos);

		pos.x += CH_X_SIZE + 1;
	}
}
