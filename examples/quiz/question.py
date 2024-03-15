import json

class QuizQuestion:
    def __init__(self,path = './utils/data.json'):
        with open(path) as f:
            self.data = json.load(f)

    def return_question(self):

        for questions in self.data:
            question = questions['question']['text']
            r_answer = questions['correctAnswer']
            w_answers = questions['incorrectAnswers']
            yield [question, r_answer, w_answers]


if __name__ == "__main__":
    q = QuizQuestion()
    for i in q.return_question():
        print(i)
