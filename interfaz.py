from tkinter import Tk, Label, Button, Entry
import subprocess
import os

vent = Tk()
vent.title("Intento interfaz1")
vent.geometry("800x400")

def turtle1():
    os.system("cd /opt/ros/humble")
    os.system("ros2 run turtlesim turtlesim_node")
def turtle2():
    subprocess.call('ros2 run', 'turtlesim','turtle_teleop_key')
lbl1 = Label(vent, text="Abrir turtlesim", bg="black", fg="white")
lbl1.place(x=295, y=185, width=100, height=30)

btn1=Button(vent, text="Turtlesim", command=turtle1)
btn1.place(x=295, y=145, width=100, height=30)

lbl2 = Label(vent, text="Mover turtlesim", bg="black", fg="white")
lbl2.place(x=405, y=185, width=100, height=30)

btn2=Button(vent, text="Mover Turtlesim", command=turtle2)
btn2.place(x=405, y=145, width=100, height=30)

vent.mainloop(
