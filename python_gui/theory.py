import tkinter as tk
import json

# загружаем вопросы из questions.json
with open("questions.json", "r", encoding="utf-8") as f:
    questions_data = json.load(f)

class TheoryScreen:
    def __init__(self, win, back_callback):
        self.win = win
        self.back_callback = back_callback

        self.frame = tk.Frame(win)

        # Кнопка Назад
        button_frame = tk.Frame(self.frame, pady=10)
        button_frame.pack(side="bottom")

        btn_menu = tk.Button(button_frame, text="Главное меню", command=self.go_back)
        btn_menu.pack(side="left", padx=10)

        # Скролл
        self.canvas = tk.Canvas(self.frame, height=400)
        scrollbar = tk.Scrollbar(self.frame, orient="vertical", command=self.canvas.yview)
        scrollable_frame = tk.Frame(self.canvas)

        # Привязка области прокрутки
        scrollable_frame.bind(
            "<Configure>",
            lambda e: self.canvas.configure(scrollregion=self.canvas.bbox("all"))
        )

        self.canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        self.canvas.configure(yscrollcommand=scrollbar.set)  # чтобы scrollbar синхронизировался с прокруткой canvas

        self.canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        self.bind_mousewheel() # привязываем прокрутку

        for i, question in enumerate(questions_data, start=1):
            frame = tk.Frame(scrollable_frame, padx=10, pady=5, bd=1, relief="solid")

            tk.Label(frame, text=f"{i}) {question['question']}", font=("Arial", 12), wraplength=600, justify="left").pack(anchor="w")
            tk.Label(frame, text=f"Правильный ответ: {question['answer']}", fg="green", anchor="w").pack(anchor="w", pady=(2, 5))

            theory_frame = tk.Frame(frame)
            btn = tk.Button(frame, text="Показать теорию")
            btn.config(command=lambda b=btn, f=theory_frame, t=question['theory']: self.toggle_theory(b, f, t))
            btn.pack(anchor="w", pady=(0, 5))

            theory_frame.pack_forget()

            frame.pack(fill="x", pady=5, padx=10)

    def toggle_theory(self, button, theory_frame, theory_text):
        if theory_frame.winfo_ismapped():
            theory_frame.pack_forget()
            button.config(text="Показать теорию")
        else:
            for widget in theory_frame.winfo_children():
                widget.destroy()
            tk.Label(theory_frame, text=theory_text, wraplength=600, justify="left").pack()
            theory_frame.pack(fill="x", pady=(0, 10))
            button.config(text="Скрыть теорию")

    def go_back(self):
        self.frame.pack_forget()
        self.back_callback()

    def show(self):
        self.frame.pack(fill="both", expand=True)

    def bind_mousewheel(self):
        # ВНУТРИ show_results добавляем функцию прокрутки
        def _on_mousewheel(event):
            # event.delta — значение прокрутки колёсика мыши. Обычно это ±120.
            self.canvas.yview_scroll(int(-1 * (event.delta / 120)),
                                "units")  # "units" — означает «прокручивать на единицу по строкам/ячейкам»

        # Привязка события колёсика мыши к canvas
        self.canvas.bind_all("<MouseWheel>", _on_mousewheel)