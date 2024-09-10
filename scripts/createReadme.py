README_PATH = "bin/readme.txt"
README_TEXT =  """\
# Keys ====================
# Alphanumeric keys =======
39   ->  Key: '
44   ->  Key: ,
45   ->  Key: -
46   ->  Key: .
47   ->  Key: /
48   ->  Key: 0
49   ->  Key: 1
50   ->  Key: 2
51   ->  Key: 3
52   ->  Key: 4
53   ->  Key: 5
54   ->  Key: 6
55   ->  Key: 7
56   ->  Key: 8
57   ->  Key: 9
59   ->  Key: ;
61   ->  Key: =
65   ->  Key: A | a
66   ->  Key: B | b
67   ->  Key: C | c
68   ->  Key: D | d
69   ->  Key: E | e
70   ->  Key: F | f
71   ->  Key: G | g
72   ->  Key: H | h
73   ->  Key: I | i
74   ->  Key: J | j
75   ->  Key: K | k
76   ->  Key: L | l
77   ->  Key: M | m
78   ->  Key: N | n
79   ->  Key: O | o
80   ->  Key: P | p
81   ->  Key: Q | q
82   ->  Key: R | r
83   ->  Key: S | s
84   ->  Key: T | t
85   ->  Key: U | u
86   ->  Key: V | v
87   ->  Key: W | w
88   ->  Key: X | x
89   ->  Key: Y | y
90   ->  Key: Z | z
91   ->  Key: [
92   ->  Key: '\'
93   ->  Key: ]
96   ->  Key: `
# Function keys ===========
32   ->  Key: Space
256  ->  Key: Esc
257  ->  Key: Enter
258  ->  Key: Tab
259  ->  Key: Backspace
260  ->  Key: Ins
261  ->  Key: Del
262  ->  Key: Cursor right
263  ->  Key: Cursor left
264  ->  Key: Cursor down
265  ->  Key: Cursor up
266  ->  Key: Page up
267  ->  Key: Page down
268  ->  Key: Home
269  ->  Key: End
280  ->  Key: Caps lock
281  ->  Key: Scroll down
282  ->  Key: Num lock
283  ->  Key: Print screen
284  ->  Key: Pause
290  ->  Key: F1
291  ->  Key: F2
292  ->  Key: F3
293  ->  Key: F4
294  ->  Key: F5
295  ->  Key: F6
296  ->  Key: F7
297  ->  Key: F8
298  ->  Key: F9
299  ->  Key: F10
300  ->  Key: F11
301  ->  Key: F12
340  ->  Key: Shift left
341  ->  Key: Control left
342  ->  Key: Alt left
343  ->  Key: Super left
344  ->  Key: Shift right
345  ->  Key: Control right
346  ->  Key: Alt right
347  ->  Key: Super right
348  ->  Key: KB menu
# Keypad keys =============
320  ->  Key: Keypad 0
321  ->  Key: Keypad 1
322  ->  Key: Keypad 2
323  ->  Key: Keypad 3
324  ->  Key: Keypad 4
325  ->  Key: Keypad 5
326  ->  Key: Keypad 6
327  ->  Key: Keypad 7
328  ->  Key: Keypad 8
329  ->  Key: Keypad 9
330  ->  Key: Keypad .
331  ->  Key: Keypad /
332  ->  Key: Keypad *
333  ->  Key: Keypad -
334  ->  Key: Keypad +
335  ->  Key: Keypad Enter
336  ->  Key: Keypad =\n"""


def main():
    print(f"Writing {README_PATH}...")
    with open(README_PATH, 'wt') as f:
        f.write(README_TEXT)

if __name__ == '__main__':
    main()
