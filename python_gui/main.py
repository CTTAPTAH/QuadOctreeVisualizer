import tkinter as tk
from trees import run_quadtree, run_octree
from test import Test
from theory import TheoryScreen

# Функции кнопок
def back_to_menu():
    test_screen.hide()
    menu_frame.pack(fill="both", expand=True)

def open_test():
    menu_frame.pack_forget()
    test_screen.show()
    test_screen.bind_mousewheel()  # для работы прокрутки в тесте

def open_theory():
    menu_frame.pack_forget()
    theory_screen.show()
    theory_screen.bind_mousewheel() # для работы прокрутки в теории

# Создание главного окна
win = tk.Tk()
win.title("App")
win.geometry("700x400")

# Сохраняем ссылку на меню, чтобы можно было скрыть и показать
menu_frame = tk.Frame(win)
menu_frame.pack(fill="both", expand=True)

# Заголовок
label = tk.Label(menu_frame, text="Главное меню", font=("Arial", 16))
label.pack(pady=20)

# Кнопки
quadtree_button = tk.Button(menu_frame, text="Квадродерево", font=("Arial", 12), width=25, command=run_quadtree)
quadtree_button.pack(pady=10)

octree_button = tk.Button(menu_frame, text="Октодерево", font=("Arial", 12), width=25, command=run_octree)
octree_button.pack(pady=10)

# Кнопки продолжения
test_button = tk.Button(menu_frame, text="Тест", font=("Arial", 12), width=25, command=open_test)
test_button.pack(pady=10)

theory_button = tk.Button(menu_frame, text="Теория", font=("Arial", 12), width=25, command=open_theory)
theory_button.pack(pady=10)

# Создаём экран теста, передаём главное окно и функцию возврата
test_screen = Test(win, back_to_menu)

# Создаём экран теории, передаём главное окно и функцию возврата
theory_screen = TheoryScreen(win, back_to_menu)

# Запуск приложения
win.mainloop()
