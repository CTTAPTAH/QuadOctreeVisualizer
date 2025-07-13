import tkinter as tk
from tkinter import messagebox
import random
import json

# загружаем вопросы из questions.json
with open("questions.json", "r", encoding="utf-8") as f:
    questions_data = json.load(f)

class Test:
    def __init__(self, win, back_callback):
        self.win = win # сохраняем главное окно
        self.back_callback = back_callback  # функция возврата в меню

        self.frame = tk.Frame(win) # Создаём новый кадр
        self.test_frame = tk.Frame(self.frame)
        self.results_frame = tk.Frame(self.frame)
        # Всегда сначала показываем test_frame
        self.test_frame.pack(fill="both", expand=True)

        self.var_choice = tk.StringVar()  # Для хранения выбранного варианта
        self.var_choice.set("None")

        self.canvas = tk.Canvas(self.results_frame, height=400) # костыльно

        # Левый верхний угол — индикатор текущего вопроса
        self.label_progress = tk.Label(self.test_frame, text="", font=("Arial", 12), anchor="w", justify="left")
        self.label_progress.pack(anchor="w", padx=10, pady=(10, 0))  # Привязка к левому верхнему краю

        # Обложка вопроса
        self.label_question = tk.Label(self.test_frame, text="", wraplength=600, font=("Arial", 14), anchor="w", justify="left")
        self.label_question.pack(pady=10)

        # создаём кнопки для вариантов ответа
        self.radio_buttons = []
        for i in range(4):  # Предполагаем максимум 4 варианта ответа
            rb = tk.Radiobutton(self.test_frame, # родитель — наш фрейм
                                text="", # текст на кнопке (позже зададим вопрос)
                                variable=self.var_choice, # связываем с self.var_choice — общая переменная
                                value="", # значение, которое установится при выборе этой кнопки
                                font=("Arial", 12), # шрифт и размер
                                anchor="w", # выравнивание текста по левому краю (w = west)
                                justify="left", # текст выравнивается по левому краю (если много строк)
                                wraplength=600) # переносить строки, если ширина больше 600 пикселей
            rb.pack(fill="x", padx=20, pady=2) # размещение: по ширине окна (x), отступы по бокам и между кнопками
            self.radio_buttons.append(rb) # добавляем в список, чтобы позже обращаться к ним

        # Нижняя панель с двумя группами кнопок
        self.bottom_frame = tk.Frame(self.test_frame)
        self.bottom_frame.pack(side="bottom", fill="x", pady=10, padx=20)

        # Левая часть (Меню, Начать сначала)
        self.left_buttons_frame = tk.Frame(self.bottom_frame)
        self.left_buttons_frame.pack(side="left")

        self.restart_btn = tk.Button(self.left_buttons_frame, text="Начать сначала", command=self.restart_test)
        self.restart_btn.pack(side="left", padx=5)

        self.menu_btn = tk.Button(self.left_buttons_frame, text="Главное меню", command=back_callback)
        self.menu_btn.pack(side="left", padx=5)

        # Правая часть (Назад, Далее)
        self.right_buttons_frame = tk.Frame(self.bottom_frame)
        self.right_buttons_frame.pack(side="right")

        self.back_btn = tk.Button(self.right_buttons_frame, text="Назад", command=self.prev_question)
        self.back_btn.pack(side="left", padx=5)

        self.btn_next = tk.Button(self.right_buttons_frame, text="Далее", command=self.next_question)
        self.btn_next.pack(side="left", padx=5)

        # вспомогательные поля для работы тесты
        self.selected_questions = []  # Список из 15 отобранных вопросов
        self.current_index = 0
        self.total_questions = 10
        self.user_answers = [None] * self.total_questions  # Для хранения ответов пользователя

        self.prepare_questions()  # Вызываем метод при запуске. И создаём тест
        self.display_question(self.current_index)  # Показываем первый вопрос

    # Создание теста с вопросами
    def prepare_questions(self):
        # 1. Случайные 15 вопросов
        self.selected_questions = random.sample(questions_data, self.total_questions)

        # 2. Перемешиваем варианты у каждого
        for q in self.selected_questions:
            random.shuffle(q["options"])

    # Отображение вопроса
    def display_question(self, index):
        # Обновляем прогресс: текущий номер вопроса из 15
        self.label_progress.config(text=f"Вопрос {index + 1} / {self.total_questions}")

        # Отображаем вопрос и варианты ответа
        q = self.selected_questions[index] # получаем нужный вопрос
        self.label_question.config(text=f"{index + 1}. {q['question']}") # выводим вопрос с номером

        self.var_choice.set(self.user_answers[index] if self.user_answers[index] else "None")  # Сброс выбора
        for i, option in enumerate(q["options"]): # перебираем варианты ответа
            self.radio_buttons[i].config(text=option, # отображаем текст варианта
                                         value=option, # значение, которое будет установлено при выборе
                                         state="normal") # включаем кнопку (на всякий случай)
        # если вариантов меньше 4, то отключаем лишние радиокнопки
        for i in range(len(q["options"]), 4):
            self.radio_buttons[i].config(text="", value="", state="disabled")

        # Включаем/отключаем кнопку "Назад"
        if self.current_index == 0:
            self.back_btn.config(state="disabled")
        else:
            self.back_btn.config(state="normal")

        # Меняем "Далее" на "Завершить", если вопрос последний
        if self.current_index == self.total_questions - 1:
            self.btn_next.config(text="Завершить")
        else:
            self.btn_next.config(text="Далее")

    def show_results(self):
        # выключаем тест, включаем результаты теста
        self.test_frame.pack_forget()
        self.results_frame.pack(fill="both", expand=True)

        # Подсчёт правильных ответов
        correct_count = 0
        for i, answer in enumerate(self.user_answers):
            if answer == self.selected_questions[i]["answer"]:
                correct_count += 1

        percent = round(100 * correct_count / self.total_questions)

        # Скрываем всё с кадра
        for widget in self.results_frame.winfo_children():
            widget.pack_forget()

        # Показываем итог
        result_text = f"Вы ответили правильно на {correct_count} из {self.total_questions} вопросов.\n({percent}%)"
        label = tk.Label(self.results_frame, text=result_text, font=("Arial", 14), pady=20)
        label.pack()

        # Кнопки управления внизу
        button_frame = tk.Frame(self.results_frame, pady=10)
        button_frame.pack(side="bottom")

        btn_restart = tk.Button(button_frame, text="Начать сначала", command=self.restart_test)
        btn_restart.pack(side="left", padx=10)

        btn_menu = tk.Button(button_frame, text="Главное меню", command=self.back_callback)
        btn_menu.pack(side="left", padx=10)

        # Верх — область с результатами
        results_frame = tk.Frame(self.results_frame)
        results_frame.pack(fill="both", expand=True)

        self.canvas = tk.Canvas(results_frame, height=400)
        scrollbar = tk.Scrollbar(results_frame, orient="vertical", command=self.canvas.yview)
        scrollable_frame = tk.Frame(self.canvas)

        # Привязка области прокрутки
        scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all"))
        )

        self.canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=scrollbar.set) # чтобы scrollbar синхронизировался с прокруткой canvas

        self.canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        # ВНУТРИ show_results добавляем функцию прокрутки
        def _on_mousewheel(event):
            # event.delta — значение прокрутки колёсика мыши. Обычно это ±120.
            self.canvas.yview_scroll(int(-1 * (event.delta / 120)), "units") # "units" — означает «прокручивать на единицу по строкам/ячейкам»

        # Привязка события колёсика мыши к canvas
        self.canvas.bind_all("<MouseWheel>", _on_mousewheel)

        # Вывод каждого вопроса
        for i, question in enumerate(self.selected_questions):
            user_ans = self.user_answers[i]
            correct_ans = question["answer"]

            # Подпись: правильно ли
            if user_ans == correct_ans:
                color = "green"
                status = "✔ Верно"
            else:
                color = "red"
                status = f"✘ Неверно"

            # Отображение вопроса и ответа
            frame = tk.Frame(scrollable_frame, padx=10, pady=5, bd=1, relief="solid") # bd=1, relief="solid" - делает рамку вокруг блока.
            frame.pack(fill="x", padx=10, pady=5)

            q_label = tk.Label(frame, text=f"{i + 1}. {question['question']}", font=("Arial", 11, "bold"),
                               wraplength=600, justify="left")
            q_label.pack(anchor="w")

            your_ans = tk.Label(frame, text=f"Ваш ответ: {user_ans}", fg=color, font=("Arial", 10))
            your_ans.pack(anchor="w")

            correct = tk.Label(frame, text=f"Правильный ответ: {correct_ans}", fg="black", font=("Arial", 10))
            correct.pack(anchor="w")

            status_label = tk.Label(frame, text=status, fg=color, font=("Arial", 10, "bold"))
            status_label.pack(anchor="w", pady=(2, 0))

            # Кнопка показа теории
            theory_frame = tk.Frame(frame)  # Пустой фрейм, будет заполняться при нажатии
            btn = tk.Button(frame, text="Показать теорию")
            btn.config(command=lambda b=btn, f=theory_frame, t=question['theory']: self.toggle_theory(b, f, t))
            btn.pack(anchor="w")

            # Сюда будет вставляться теория при раскрытии
            theory_frame.pack_forget()  # Сначала скрыт

    def toggle_theory(self, button, theory_frame, theory_text):
        # Если теория уже показана — скрыть
        if theory_frame.winfo_ismapped():
            theory_frame.pack_forget()
            button.config(text="Показать теорию")
        else:
            # Очистить и добавить новый текст
            for widget in theory_frame.winfo_children():
                widget.destroy()
            tk.Label(theory_frame, text=theory_text, wraplength=600, justify="left", font=("Arial", 10)).pack()
            theory_frame.pack(fill="x")
            button.config(text="Скрыть теорию")

    # переход на следующий вопрос
    def next_question(self):
        selected = self.var_choice.get()
        if selected:
            self.user_answers[self.current_index] = selected

        # проверяем хочет ли пользователь завершить тест
        if self.current_index == self.total_questions - 1:
             # Проверка на завершение теста
            unanswered = [i + 1 for i, ans in enumerate(self.user_answers) if ans == "None"]

            if unanswered:
                text = f"Вы не ответили на вопросы: {', '.join(map(str, unanswered))}.\nЗавершить тест?"
            else:
                text = f"Вы ответили на все вопросы.\nЗавершить тест?"

            if messagebox.askyesno("Завершение теста", text):
                self.show_results()

        # смещаемся по тесту, если можно
        if self.current_index < self.total_questions - 1:
            self.current_index += 1
            self.display_question(self.current_index)

    def prev_question(self):
        selected = self.var_choice.get()
        if selected:
            self.user_answers[self.current_index] = selected

        if self.current_index > 0:
            self.current_index -= 1
            self.display_question(self.current_index)

    def restart_test(self):
        # выключаем результаты теста, включаем сам тест
        self.results_frame.pack_forget()
        self.test_frame.pack(fill="both", expand=True)

        self.current_index = 0
        self.user_answers = [None] * self.total_questions  # Для хранения ответов пользователя
        self.prepare_questions()  # Вызываем метод при запуске. И создаём тест
        self.display_question(self.current_index)  # Показываем первый вопрос

    def show(self):
        self.frame.pack(fill="both", expand=True) # Показываем этот "экран" (frame)

    def hide(self):
        self.frame.pack_forget() # Скрываем этот "экран"

    def bind_mousewheel(self):
        # ВНУТРИ show_results добавляем функцию прокрутки
        def _on_mousewheel(event):
            # event.delta — значение прокрутки колёсика мыши. Обычно это ±120.
            self.canvas.yview_scroll(int(-1 * (event.delta / 120)),
                                "units")  # "units" — означает «прокручивать на единицу по строкам/ячейкам»

        # Привязка события колёсика мыши к canvas
        self.canvas.bind_all("<MouseWheel>", _on_mousewheel)