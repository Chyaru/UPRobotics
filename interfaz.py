import tkinter as tk

def mostrar_mensaje():
    etiqueta.config(text="Hola, Mundo!")

# Crear la ventana
ventana = tk.Tk()
ventana.title("Interfaz Simple")

# Crear un botón
boton = tk.Button(ventana, text="Hacer clic", command=mostrar_mensaje)
boton.pack(pady=10)

# Crear una etiqueta para mostrar un mensaje
etiqueta = tk.Label(ventana, text="")
etiqueta.pack()

# Iniciar el bucle principal de la interfaz gráfica
ventana.mainloop()
