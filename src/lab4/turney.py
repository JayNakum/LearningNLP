import math
import re
import json
import nltk
nltk.download('averaged_perceptron_tagger',quiet=True)


def loadReviews(fileName):
    list_pos = []
    list_neg = []

    data = []
    with open(fileName, 'r') as f:
        for line in f:
            data.append(json.loads(line))

    for elem in data:
        if float(elem["overall"]) >= 3.0:
            list_pos.append(elem["reviewText"])
        else:
            list_neg.append(elem["reviewText"])

    return list_pos, list_neg


def make_datasets(fileName):
    all_positive_reviews, all_negative_reviews = loadReviews(fileName)
    dataset = {'train': {'neg': [], 'pos': []}, 'test': {'neg': [], 'pos': []}}
    dataset['train']['pos'] = (all_positive_reviews[:20000])
    dataset['train']['neg'] = (all_negative_reviews[:20000])
    dataset['test']['pos'] = (all_positive_reviews[-50:])
    dataset['test']['neg'] = (all_negative_reviews[-50:])

    return dataset


def find_pattern(postag):
    tag_pattern = []
    for k in range(len(postag) - 2):
        if postag[k][1] == "JJ" and (postag[k + 1][1] == "NN" or postag[k + 1][1] == "NNS"):
            tag_pattern.append("".join(postag[k][0]) + " " + "".join(postag[k + 1][0]))

        elif ((postag[k][1] == "RB" or postag[k][1] == "RBR" or postag[k][1] == "RBS") and postag[k + 1][1] == "JJ" and
              postag[k + 2][1] != "NN" and postag[k + 2][1] != "NNS"):
            tag_pattern.append("".join(postag[k][0]) + " " + "".join(postag[k + 1][0]))

        elif postag[k][1] == "JJ" and postag[k + 1][1] == "JJ" and postag[k + 2][1] != "NN" and postag[k + 2][
            1] != "NNS":
            tag_pattern.append("".join(postag[k][0]) + " " + "".join(postag[k + 1][0]))

        elif (postag[k][1] == "NN" or postag[k][1] == "NNS") and postag[k + 1][1] == "JJ" and postag[k + 2][
            1] != "NN" and postag[k + 2][1] != "NNS":
            tag_pattern.append("".join(postag[k][0]) + " " + "".join(postag[k + 1][0]))

        elif ((postag[k][1] == "RB" or postag[k][1] == "RBR" or postag[k][1] == "RBS") and (
                postag[k + 1][1] == "VB" or postag[k + 1][1] == "VBD" or postag[k + 1][1] == "VBN" or postag[k + 1][
            1] == "VBG")):
            tag_pattern.append("".join(postag[k][0]) + " " + "".join(postag[k + 1][0]))
    return tag_pattern


def near_operator(phrase, word, text):
    try:
        string = word + r'\W+(?:\w+\W+){0,400}?' + phrase + r'|' + phrase + r'\W+(?:\w+\W+){0,400}?' + word
        freq_phrase_near_word = (len(re.findall(string, text)))
        return freq_phrase_near_word
    except:
        return 0


class Turney(object):

    def __init__(self, dataset):
        self.datasets = dataset
        self.pos_phrases_hits = []
        self.neg_phrases_hits = []
        self.pos_hits = 0.01
        self.neg_hits = 0.01
        self.accuracy = 0

    def turney(self):
        tp = 0
        fp = 0
        tn = 0
        fn = 0
        for boolean, test_klass in enumerate(['pos', 'neg']):
            for i, data in enumerate(self.datasets['test'][test_klass]):
                print(str(i) + " out of " + str(len(self.datasets['test'][test_klass])) + " --> round " + str(boolean))

                phrases = find_pattern(nltk.pos_tag(nltk.word_tokenize(data)))
                if len(phrases) == 0:
                    continue
                self.pos_phrases_hits = [0.01] * len(phrases)
                self.neg_phrases_hits = [0.01] * len(phrases)
                self.pos_hits = 0.01
                self.neg_hits = 0.01

                for train_klass in ['pos', 'neg']:
                    for text in self.datasets['train'][train_klass]:
                        for ind, phrase in enumerate(phrases):
                            self.pos_phrases_hits[ind] += near_operator(phrase, "excellent", text)
                            self.neg_phrases_hits[ind] += near_operator(phrase, "poor", text)
                            self.pos_hits += text.count("excellent")
                            self.neg_hits += text.count("poor")
                res = self.calculate_sentiment(boolean)
                # compute if correct prediction
                if res == 1 and boolean == 0:
                    fp += 1
                elif res == 1 and boolean == 1:
                    tp += 1
                elif res == 0 and boolean == 0:
                    fn += 1
                elif res == 0 and boolean == 1:
                    tn += 1

        print("Accuracy: " + str(self.accuracy / 100))
        print("True positive: " + str(tp))
        print("False positive: " + str(fp))
        print("True negative: " + str(tn))
        print("False negative: " + str(fn))
        print("Recall-positive: " + str(tp / (tp + fn)))
        print("Precision-positive: " + str(tp / (tp + fp)))
        print("Recall-negative: " + str(tn / (tn + fp)))
        print("Precision-negative: " + str(tn / (tn + fn)))

    def calculate_sentiment(self, is_negative=0):
        polarities = [0] * len(self.pos_phrases_hits)
        for i in range(len(self.pos_phrases_hits)):
            polarities[i] = math.log(
                (self.pos_phrases_hits[i] * self.neg_hits) / (self.neg_phrases_hits[i] * self.pos_hits), 2)
        pmi = sum(polarities) / len(polarities)
        if (pmi > 0 and is_negative == 0) or (pmi < 0 and is_negative == 1):
            self.accuracy += 1
            return 1
        return 0


if __name__ == "__main__":
    FILE_PATH = './Cell_Phones_and_Accessories_5.json'
    datasets = make_datasets(FILE_PATH)
    turney = Turney(datasets)
    turney.turney()
