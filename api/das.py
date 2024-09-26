import turtle
t = turtle.Turtle()
a = 0
b = 1
t.speed(999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999)
while True:
    a += 1
    t.forward(10)
    t.left(3)
    if a == 30:
        t.left(90)
    if a == 60:
        t.left(179)
        a = 0