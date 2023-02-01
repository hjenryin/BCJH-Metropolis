import json
from typing import List, Tuple


class NoIntentError(Exception):
    pass


def addEndline(s: str) -> str:
    if s != "":
        if not s.endswith("\n"):
            return s+"\n"
    return s


class Code:
    def __init__(self, head: str, end: str):
        self.head = head
        self.end = end
        self.body = None

    def add(self, head: str, end: str):
        self.body = Code(head, end)
        return self.body

    def addFront(self, word: str):
        self.head = addEndline(self.head)+word
        return self

    def addBack(self, word: str):

        self.end = addEndline(word)+self.end
        return self

    def print(self) -> str:

        s = addEndline(self.head)
        if self.body:
            s += addEndline(self.body.print())
        s += self.end

        return s


head = """
// 本代码由 ./src/generateRule.py 自动生成
#ifndef RULE_HPP
#define RULE_HPP
#include "src/Calculator.hpp"

/**
 * @brief
 *
 * @param rule
 * @param s
 * @return int : the optimal `full` value
 */
int banquetRule(BanquetRule *const &rule, States &s) {
    BanquetStrictRule
        *strictRule[NUM_CHEFS *
                    DISH_PER_CHEF]; // 下一阶段的规则，不受意图生效次数的影响
    BanquetLenientRule *lenientRule[NUM_CHEFS * DISH_PER_CHEF];
    for (int i = 0; i < NUM_CHEFS * DISH_PER_CHEF; i++) {
        strictRule[i] = &rule[i].strictRule;
        lenientRule[i] = &rule[i].lenientRule;
    }

"""
tail = """
    
}

#endif
"""

with open("./data/data.min.json", "r") as f:
    data = json.load(f)

    if "intents" in data.keys():
        intentions = data["intents"]
        rules = data["rules"]
        buffs = data["buffs"]
    else:
        raise "文件中没有宴会规则。"


def getIntention(intentId):
    for i in intentions:
        if i["intentId"] == intentId:
            return i


def getBuff(buffId):
    for i in buffs:
        if i["buffId"] == buffId:
            return i


def getRule():
    global head
    global tail
    for rule in rules:
        if "IntentList" in rule.keys():
            # print(rule["IntentList"])
            head = "// "+rule["Title"]+"\n"+head
            tail = "return "+str(rule["Satiety"])+";\n"+tail

            return rule


def parseConditionValue(v):
    # print(v)
    ability = ["Fry", "Boil", "Steam", "Bake", "Stirfry", "Knife"]
    flavor = ["Sweet", "Spicy", "Salty", "Tasty", "Sour", "Bitter"]
    if v in ability:
        return "ability", v.lower()
    else:
        raise NotImplementedError


def parseCondition(roundNum, offset, rule, strict):
    end = "break;}"
    if "lastRounds" in rule.keys():
        if rule["lastRounds"] == 1:
            end = "}"
        else:
            raise NotImplementedError
    i = "i"
    if offset:
        i = "i+"+str(offset)
    s = ""
    if "desc" in rule.keys():
        s = "// 条件："+rule["desc"]
    c0 = Code(s, "")
    c = c0
    ct = rule["conditionType"]
    cv = rule["conditionValue"]
    if ct == "Group":
        strict = True
        t, v = parseConditionValue(cv)
        if t == "ability":
            c = c.add("if (s.recipe[%d]->cookAbility.%s > 0 && s.recipe[%d]->cookAbility.%s > 0 && s.recipe[%d]->cookAbility.%s > 0) {" % (
                roundNum, v, roundNum+1, v, roundNum+2, v), "}")
            word = parseEffect(roundNum, offset, rule, strict)
            c = c.add(word, "")

        else:
            raise NotImplementedError
    else:
        if not offset:
            c = c.add("for (int i = %d; i < %d; i++) {" %
                      (roundNum*3, roundNum*3+3), "}")
        if ct == "Order":
            c = c.add("if (%s == %d) {" % (i, roundNum*3+int(cv)-1), end)
        elif ct == "Rarity":
            c = c.add(
                "if (s.recipe[%s]->rarity == %d) {" % (i, int(cv)), end)
        elif ct == "CondimentSkill":
            c = c.add(
                "if (s.recipe[%s]->flavor.%s == true) {" % (i, cv.lower()), end)
        elif ct == "CookSkill":
            c = c.add("if (s.recipe[%s]->cookAbility.%s > 0) {" % (i,
                                                                   cv.lower()), end)
        else:
            raise NotImplementedError
        word = parseEffect(roundNum, offset, rule, strict)
        c = c.add(word, "")
    return c0.print()


def parseEffect(roundNum, offset, rule, strict):
    r = ""
    i = "i"
    if offset:
        i = "i+" + str(offset)
    if strict:
        # print("r set strict")
        r = f"strictRule[{i}]->"
    else:
        r = f"lenientRule[{i}]->"
    s = ""
    if "desc" in rule.keys():
        s = "// 效果："+rule["desc"]
    c0 = Code(s, "")
    c = c0
    et = rule["effectType"]
    ev = rule["effectValue"]
    if et == "SatietyChange":
        c = c.add(r+"addRule.full+= %d;" % int(ev), "")
    elif et == "PriceChangePercent":
        c = c.add(r+"addRule.buff+= %d;" % int(ev), "")
    elif et == "IntentAdd":
        if ev == 100:
            # print("oneMore")
            c = c.add(r+"oneMore();", "")
        else:
            raise NotImplementedError
    elif et == "CreateIntent":
        # 是不是就是为下道料理create？
        c = c.add("if (i+1<%d){" % (roundNum*3+3), "}")
        word = parseRule(roundNum, 1, getIntention(ev), strict)
        c = c.add(word, "")
    elif et == "CreateBuff":
        word = parseRule(roundNum+1, None, getBuff(ev), strict)
        c = c.add(word, "")
    elif et == "BasicPriceChangePercent":
        c = c.add(r+"baseRule.buff+= %d;" % int(ev), "")
    elif et == "BasicPriceChange":
        c = c.add(r+"baseRule.directAdd+= %d;" % int(ev), "")
    elif et == "SetSatietyValue":
        c = c.add(r+"addRule.full=%d;" % int(ev)+r +
                  "addRule.fullAdd=false;", "")
    else:
        print(et)
        raise NotImplementedError
    return c0.print()


def parseRule(roundNum, offset, rule, strict) -> Tuple[str, bool]:
    c0 = Code("", "")
    c = c0
    et = rule["effectType"]
    if "conditionType" in rule.keys():
        word = parseCondition(roundNum, offset, rule, strict)
        c = c.add(word, "")
    else:
        word = parseEffect(roundNum, offset, rule, strict)
        c = c.add(word, "")

    return c0.print()


def parseRules(roundNum, roundRules):
    c0 = Code(f"// 第{roundNum+1}轮", "")
    c = c0
    for rule in roundRules:
        word = parseRule(roundNum, None, getIntention(rule), False)
        c = c.addBack(word)

    return c0.print()


def generateRule():
    rule = getRule()["IntentList"]  # 会改head,tail，要在Code前面调用
    c0 = Code(head, tail)
    for i, roundRule in enumerate(rule):
        c = c0
        c = c.addFront(parseRules(i, roundRule))

    print(c0.print())


try:
    generateRule()
except Exception as e:
    if isinstance(e, NotImplementedError):
        print("出现了未知的新规则，需要手动实现。")
    else:
        print("出现了未知的错误。")
        print(e)
