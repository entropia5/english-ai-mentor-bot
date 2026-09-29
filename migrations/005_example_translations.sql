-- Add translations only to unchanged catalog examples, including learned words.
-- IDs, original examples, user data and all progress fields remain intact.
UPDATE words AS w
SET definition_ru = w.definition_ru || v.suffix
FROM (VALUES
('conversation','i','Первые фразы: кто я и что мне нужно
Пример: I am ready.','
Перевод: Я готов.'),
('conversation','you','Первые фразы: кто я и что мне нужно
Пример: You can help me.','
Перевод: Вы можете помочь мне.'),
('conversation','be','Первые фразы: кто я и что мне нужно
Пример: I want to be here.','
Перевод: Я хочу быть здесь.'),
('conversation','have','Первые фразы: кто я и что мне нужно
Пример: I have a question.','
Перевод: У меня есть вопрос.'),
('conversation','do','Первые фразы: кто я и что мне нужно
Пример: What do you do?','
Перевод: Чем вы занимаетесь?'),
('conversation','go','Первые фразы: кто я и что мне нужно
Пример: I go to work by bus.','
Перевод: Я езжу на работу на автобусе.'),
('conversation','want','Первые фразы: кто я и что мне нужно
Пример: I want some water.','
Перевод: Я хочу воды.'),
('conversation','need','Первые фразы: кто я и что мне нужно
Пример: I need your help.','
Перевод: Мне нужна ваша помощь.'),
('conversation','can','Первые фразы: кто я и что мне нужно
Пример: Can you help me?','
Перевод: Вы можете мне помочь?'),
('conversation','help','Первые фразы: кто я и что мне нужно
Пример: Please help me.','
Перевод: Пожалуйста, помогите мне.'),
('conversation','please','Первые фразы: кто я и что мне нужно
Пример: Please speak slowly.','
Перевод: Пожалуйста, говорите медленнее.'),
('conversation','thank','Первые фразы: кто я и что мне нужно
Пример: Thank you for your help.','
Перевод: Спасибо за вашу помощь.'),
('conversation','yes','Первые фразы: кто я и что мне нужно
Пример: Yes, I understand.','
Перевод: Да, я понимаю.'),
('conversation','no','Первые фразы: кто я и что мне нужно
Пример: No, thank you.','
Перевод: Нет, спасибо.'),
('conversation','not','Первые фразы: кто я и что мне нужно
Пример: I am not ready.','
Перевод: Я не готов.'),
('conversation','hello','Первые фразы: кто я и что мне нужно
Пример: Hello, my name is Alex.','
Перевод: Здравствуйте, меня зовут Алекс.'),
('conversation','sorry','Первые фразы: кто я и что мне нужно
Пример: Sorry, I am late.','
Перевод: Извините, я опоздал.'),
('conversation','excuse','Первые фразы: кто я и что мне нужно
Пример: Excuse me, is this seat free?','
Перевод: Извините, это место свободно?'),
('conversation','understand','Первые фразы: кто я и что мне нужно
Пример: I understand the question.','
Перевод: Я понимаю вопрос.'),
('conversation','know','Первые фразы: кто я и что мне нужно
Пример: I do not know.','
Перевод: Я не знаю.'),
('conversation','speak','Первые фразы: кто я и что мне нужно
Пример: Do you speak English?','
Перевод: Вы говорите по-английски?'),
('conversation','say','Первые фразы: кто я и что мне нужно
Пример: What did you say?','
Перевод: Что вы сказали?'),
('conversation','tell','Первые фразы: кто я и что мне нужно
Пример: Tell me your name.','
Перевод: Скажите, как вас зовут.'),
('conversation','ask','Первые фразы: кто я и что мне нужно
Пример: Can I ask a question?','
Перевод: Можно задать вопрос?'),
('conversation','answer','Первые фразы: кто я и что мне нужно
Пример: I do not know the answer.','
Перевод: Я не знаю ответа.'),
('conversation','question','Первые фразы: кто я и что мне нужно
Пример: I have one question.','
Перевод: У меня один вопрос.'),
('conversation','name','Первые фразы: кто я и что мне нужно
Пример: What is your name?','
Перевод: Как вас зовут?'),
('conversation','from','Первые фразы: кто я и что мне нужно
Пример: Where are you from?','
Перевод: Откуда вы?'),
('conversation','live','Первые фразы: кто я и что мне нужно
Пример: I live near here.','
Перевод: Я живу рядом отсюда.'),
('conversation','here','Первые фразы: кто я и что мне нужно
Пример: Please sit here.','
Перевод: Пожалуйста, садитесь здесь.'),
('conversation','he','Местоимения и первые связи
Пример: He is my friend.','
Перевод: Он мой друг.'),
('conversation','she','Местоимения и первые связи
Пример: She works here.','
Перевод: Она работает здесь.'),
('conversation','it','Местоимения и первые связи
Пример: It is easy.','
Перевод: Это легко.'),
('conversation','we','Местоимения и первые связи
Пример: We are ready.','
Перевод: Мы готовы.'),
('conversation','they','Местоимения и первые связи
Пример: They live nearby.','
Перевод: Они живут рядом.'),
('conversation','me','Местоимения и первые связи
Пример: Call me tomorrow.','
Перевод: Позвоните мне завтра.'),
('conversation','him','Местоимения и первые связи
Пример: I know him.','
Перевод: Я его знаю.'),
('conversation','her','Местоимения и первые связи
Пример: I work with her.','
Перевод: Я работаю с ней.'),
('conversation','us','Местоимения и первые связи
Пример: Come with us.','
Перевод: Идите с нами.'),
('conversation','them','Местоимения и первые связи
Пример: I will ask them.','
Перевод: Я спрошу их.'),
('conversation','my','Местоимения и первые связи
Пример: This is my bag.','
Перевод: Это моя сумка.'),
('conversation','your','Местоимения и первые связи
Пример: Is this your phone?','
Перевод: Это ваш телефон?'),
('conversation','his','Местоимения и первые связи
Пример: What is his name?','
Перевод: Как его зовут?'),
('conversation','its','Местоимения и первые связи
Пример: The shop changed its name.','
Перевод: Магазин изменил своё название.'),
('conversation','our','Местоимения и первые связи
Пример: This is our home.','
Перевод: Это наш дом.'),
('conversation','their','Местоимения и первые связи
Пример: Their house is nearby.','
Перевод: Их дом рядом.'),
('conversation','this','Местоимения и первые связи
Пример: This is my seat.','
Перевод: Это моё место.'),
('conversation','that','Местоимения и первые связи
Пример: That sounds good.','
Перевод: Звучит хорошо.'),
('conversation','these','Местоимения и первые связи
Пример: These shoes fit.','
Перевод: Эти туфли подходят.'),
('conversation','those','Местоимения и первые связи
Пример: Are those your keys?','
Перевод: Это ваши ключи?'),
('conversation','a','Местоимения и первые связи
Пример: I need a pen.','
Перевод: Мне нужна ручка.'),
('conversation','an','Местоимения и первые связи
Пример: She has an idea.','
Перевод: У неё есть идея.'),
('conversation','the','Местоимения и первые связи
Пример: Please close the door.','
Перевод: Пожалуйста, закройте дверь.'),
('conversation','and','Местоимения и первые связи
Пример: Tea and coffee, please.','
Перевод: Чай и кофе, пожалуйста.'),
('conversation','but','Местоимения и первые связи
Пример: I am tired, but I can help.','
Перевод: Я устал, но могу помочь.'),
('conversation','or','Местоимения и первые связи
Пример: Tea or coffee?','
Перевод: Чай или кофе?'),
('conversation','because','Местоимения и первые связи
Пример: I stayed home because I was tired.','
Перевод: Я остался дома, потому что был уставшим.'),
('conversation','if','Местоимения и первые связи
Пример: Call me if you need help.','
Перевод: Позвони мне, если тебе нужна помощь.'),
('conversation','so','Местоимения и первые связи
Пример: It is late, so let''s go.','
Перевод: Поздно, так что пойдем.'),
('conversation','with','Местоимения и первые связи
Пример: I am with my family.','
Перевод: Я с семьёй.'),
('conversation','what','Вопросы и расположение
Пример: What do you need?','
Перевод: Что тебе нужно?'),
('conversation','who','Вопросы и расположение
Пример: Who is that?','
Перевод: Кто это?'),
('conversation','where','Вопросы и расположение
Пример: Where is the station?','
Перевод: Где станция?'),
('conversation','when','Вопросы и расположение
Пример: When do you finish?','
Перевод: Когда ты заканчиваешь?'),
('conversation','why','Вопросы и расположение
Пример: Why are you leaving?','
Перевод: Почему ты уходишь?'),
('conversation','how','Вопросы и расположение
Пример: How does this work?','
Перевод: Как это работает?'),
('conversation','which','Вопросы и расположение
Пример: Which one do you want?','
Перевод: Какой ты хочешь?'),
('conversation','whose','Вопросы и расположение
Пример: Whose bag is this?','
Перевод: Чья это сумка?'),
('conversation','in','Вопросы и расположение
Пример: My keys are in my bag.','
Перевод: Мои ключи в моей сумке.'),
('conversation','on','Вопросы и расположение
Пример: Your phone is on the table.','
Перевод: Твой телефон на столе.'),
('conversation','at','Вопросы и расположение
Пример: Meet me at the station.','
Перевод: Встретимся у станции.'),
('conversation','to','Вопросы и расположение
Пример: I want to go home.','
Перевод: Я хочу домой.'),
('conversation','for','Вопросы и расположение
Пример: This is for you.','
Перевод: Это для тебя.'),
('conversation','of','Вопросы и расположение
Пример: A glass of water, please.','
Перевод: Стакан воды, пожалуйста.'),
('conversation','about','Вопросы и расположение
Пример: Tell me about your work.','
Перевод: Расскажи мне о своей работе.'),
('conversation','by','Вопросы и расположение
Пример: I travel by train.','
Перевод: Я путешествую поездом.'),
('conversation','without','Вопросы и расположение
Пример: Coffee without sugar, please.','
Перевод: Кофе без сахара, пожалуйста.'),
('conversation','under','Вопросы и расположение
Пример: The bag is under the chair.','
Перевод: Сумка под стулом.'),
('conversation','over','Вопросы и расположение
Пример: The plane flew over the city.','
Перевод: Самолёт пролетел над городом.'),
('conversation','between','Вопросы и расположение
Пример: Sit between us.','
Перевод: Сядь между нами.'),
('conversation','near','Вопросы и расположение
Пример: I live near the park.','
Перевод: Я живу рядом с парком.'),
('conversation','behind','Вопросы и расположение
Пример: The car is behind the house.','
Перевод: Машина находится за домом.'),
('conversation','inside','Вопросы и расположение
Пример: Let''s wait inside.','
Перевод: Давайте подождём внутри.'),
('conversation','outside','Вопросы и расположение
Пример: It is cold outside.','
Перевод: На улице холодно.'),
('conversation','up','Вопросы и расположение
Пример: Look up.','
Перевод: Посмотри вверх.'),
('conversation','down','Вопросы и расположение
Пример: Please sit down.','
Перевод: Сядьте, пожалуйста.'),
('conversation','out','Вопросы и расположение
Пример: Let''s go out.','
Перевод: Давайте выйдем.'),
('conversation','there','Вопросы и расположение
Пример: Put it there.','
Перевод: Положите это туда.'),
('conversation','away','Вопросы и расположение
Пример: The station is far away.','
Перевод: Вокзал находится далеко.'),
('conversation','back','Вопросы и расположение
Пример: I will be back soon.','
Перевод: Я скоро вернусь.'),
('conversation','get','Самые нужные действия
Пример: How do I get there?','
Перевод: Как мне туда добраться?'),
('conversation','make','Самые нужные действия
Пример: Can you make some tea?','
Перевод: Вы можете приготовить чай?'),
('conversation','take','Самые нужные действия
Пример: Take your time.','
Перевод: Не спешите.'),
('conversation','give','Самые нужные действия
Пример: Give me a minute.','
Перевод: Дайте мне минуту.'),
('conversation','come','Самые нужные действия
Пример: Come here, please.','
Перевод: Подойдите сюда, пожалуйста.'),
('conversation','see','Самые нужные действия
Пример: I can see you.','
Перевод: Я вас вижу.'),
('conversation','look','Самые нужные действия
Пример: Look at this.','
Перевод: Посмотрите на это.'),
('conversation','hear','Самые нужные действия
Пример: Can you hear me?','
Перевод: Вы меня слышите?'),
('conversation','listen','Самые нужные действия
Пример: Please listen to me.','
Перевод: Пожалуйста, послушайте меня.'),
('conversation','think','Самые нужные действия
Пример: I think it is a good idea.','
Перевод: Я думаю, что это хорошая идея.'),
('conversation','feel','Самые нужные действия
Пример: I feel better today.','
Перевод: Сегодня я чувствую себя лучше.'),
('conversation','like','Самые нужные действия
Пример: I like this place.','
Перевод: Мне нравится это место.'),
('conversation','love','Самые нужные действия
Пример: I love my family.','
Перевод: Я люблю свою семью.'),
('conversation','work','Самые нужные действия
Пример: I work in a shop.','
Перевод: Я работаю в магазине.'),
('conversation','use','Самые нужные действия
Пример: Can I use your phone?','
Перевод: Можно воспользоваться вашим телефоном?'),
('conversation','try','Самые нужные действия
Пример: Let me try.','
Перевод: Позвольте мне попробовать.'),
('conversation','learn','Самые нужные действия
Пример: I want to learn English.','
Перевод: Я хочу выучить английский.'),
('conversation','read','Самые нужные действия
Пример: Can you read this?','
Перевод: Вы можете прочитать это?'),
('conversation','write','Самые нужные действия
Пример: Write your name here.','
Перевод: Напишите здесь своё имя.'),
('conversation','find','Самые нужные действия
Пример: I cannot find my keys.','
Перевод: Я не могу найти свои ключи.'),
('conversation','leave','Самые нужные действия
Пример: What time do we leave?','
Перевод: Во сколько мы уходим?'),
('conversation','stay','Самые нужные действия
Пример: Can I stay here?','
Перевод: Можно мне остаться здесь?'),
('conversation','keep','Самые нужные действия
Пример: Keep the receipt.','
Перевод: Сохраните чек.'),
('conversation','put','Самые нужные действия
Пример: Put it on the table.','
Перевод: Положите это на стол.'),
('conversation','bring','Самые нужные действия
Пример: Bring your passport.','
Перевод: Возьмите паспорт.'),
('conversation','buy','Самые нужные действия
Пример: I need to buy food.','
Перевод: Мне нужно купить еду.'),
('conversation','pay','Самые нужные действия
Пример: Can I pay by card?','
Перевод: Можно оплатить картой?'),
('conversation','wait','Самые нужные действия
Пример: Please wait for me.','
Перевод: Подождите меня, пожалуйста.'),
('conversation','meet','Самые нужные действия
Пример: Nice to meet you.','
Перевод: Приятно познакомиться.'),
('conversation','call','Самые нужные действия
Пример: Call me tonight.','
Перевод: Позвоните мне сегодня вечером.'),
('conversation','now','Говорим о времени
Пример: I am busy now.','
Перевод: Я сейчас занят.'),
('conversation','then','Говорим о времени
Пример: Eat first, then we can leave.','
Перевод: Сначала поешь, а потом мы сможем уйти.'),
('conversation','today','Говорим о времени
Пример: I work today.','
Перевод: Я работаю сегодня.'),
('conversation','tomorrow','Говорим о времени
Пример: See you tomorrow.','
Перевод: Увидимся завтра.'),
('conversation','yesterday','Говорим о времени
Пример: I called you yesterday.','
Перевод: Я позвонил тебе вчера.'),
('conversation','soon','Говорим о времени
Пример: I will be there soon.','
Перевод: Я скоро буду там.'),
('conversation','later','Говорим о времени
Пример: Let''s talk later.','
Перевод: Поговорим позже.'),
('conversation','early','Говорим о времени
Пример: I wake up early.','
Перевод: Я просыпаюсь рано.'),
('conversation','late','Говорим о времени
Пример: Sorry, I am late.','
Перевод: Извините, я опоздал.'),
('conversation','always','Говорим о времени
Пример: I always carry water.','
Перевод: Я всегда ношу воду.'),
('conversation','usually','Говорим о времени
Пример: I usually walk to work.','
Перевод: Я обычно хожу на работу пешком.'),
('conversation','often','Говорим о времени
Пример: I often cook at home.','
Перевод: Я часто готовлю дома.'),
('conversation','sometimes','Говорим о времени
Пример: I sometimes work at night.','
Перевод: Я иногда работаю ночью.'),
('conversation','never','Говорим о времени
Пример: I never smoke.','
Перевод: Я никогда не курю.'),
('conversation','again','Говорим о времени
Пример: Please say that again.','
Перевод: Пожалуйста, скажите это ещё раз.'),
('conversation','already','Говорим о времени
Пример: I have already eaten.','
Перевод: Я уже поел.'),
('conversation','still','Говорим о времени
Пример: Are you still here?','
Перевод: Ты всё ещё здесь?'),
('conversation','yet','Говорим о времени
Пример: I am not ready yet.','
Перевод: Я ещё не готов.'),
('conversation','just','Говорим о времени
Пример: I just got home.','
Перевод: Я только что пришёл домой.'),
('conversation','before','Говорим о времени
Пример: Call me before lunch.','
Перевод: Позвони мне до обеда.'),
('conversation','after','Говорим о времени
Пример: Let''s meet after work.','
Перевод: Встретимся после работы.'),
('conversation','during','Говорим о времени
Пример: I slept during the flight.','
Перевод: Я спал во время полёта.'),
('conversation','until','Говорим о времени
Пример: I work until six.','
Перевод: Я работаю до шести.'),
('conversation','since','Говорим о времени
Пример: I have lived here since May.','
Перевод: Я живу здесь с мая.'),
('conversation','time','Говорим о времени
Пример: What time is it?','
Перевод: Который час?'),
('conversation','day','Говорим о времени
Пример: Have a good day.','
Перевод: Хорошего дня.'),
('conversation','week','Говорим о времени
Пример: See you next week.','
Перевод: Увидимся на следующей неделе.'),
('conversation','month','Говорим о времени
Пример: I moved here last month.','
Перевод: Я переехал сюда в прошлом месяце.'),
('conversation','year','Говорим о времени
Пример: I visit every year.','
Перевод: Я приезжаю каждый год.'),
('conversation','hour','Говорим о времени
Пример: It takes one hour.','
Перевод: Это занимает один час.'),
('conversation','one','Количество и выбор
Пример: One ticket, please.','
Перевод: Один билет, пожалуйста.'),
('conversation','two','Количество и выбор
Пример: A table for two, please.','
Перевод: Столик на двоих, пожалуйста.'),
('conversation','three','Количество и выбор
Пример: I have three questions.','
Перевод: У меня три вопроса.'),
('conversation','four','Количество и выбор
Пример: We leave at four.','
Перевод: Мы уходим в четыре.'),
('conversation','five','Количество и выбор
Пример: Give me five minutes.','
Перевод: Дайте мне пять минут.'),
('conversation','six','Количество и выбор
Пример: I finish at six.','
Перевод: Я заканчиваю в шесть.'),
('conversation','seven','Количество и выбор
Пример: The shop opens at seven.','
Перевод: Магазин открывается в семь.'),
('conversation','eight','Количество и выбор
Пример: I sleep for eight hours.','
Перевод: Я сплю восемь часов.'),
('conversation','nine','Количество и выбор
Пример: The train leaves at nine.','
Перевод: Поезд отправляется в девять.'),
('conversation','ten','Количество и выбор
Пример: It costs ten euros.','
Перевод: Это стоит десять евро.'),
('conversation','all','Количество и выбор
Пример: Is that all?','
Перевод: Это всё?'),
('conversation','some','Количество и выбор
Пример: I need some help.','
Перевод: Мне нужна помощь.'),
('conversation','any','Количество и выбор
Пример: Do you have any questions?','
Перевод: У вас есть вопросы?'),
('conversation','many','Количество и выбор
Пример: How many tickets do you need?','
Перевод: Сколько билетов вам нужно?'),
('conversation','much','Количество и выбор
Пример: How much is it?','
Перевод: Сколько это стоит?'),
('conversation','more','Количество и выбор
Пример: I need more time.','
Перевод: Мне нужно больше времени.'),
('conversation','less','Количество и выбор
Пример: Use less sugar.','
Перевод: Используйте меньше сахара.'),
('conversation','few','Количество и выбор
Пример: I have a few friends here.','
Перевод: У меня здесь несколько друзей.'),
('conversation','little','Количество и выбор
Пример: I speak a little English.','
Перевод: Я немного говорю по‑английски.'),
('conversation','enough','Количество и выбор
Пример: We have enough time.','
Перевод: У нас достаточно времени.'),
('conversation','every','Количество и выбор
Пример: I walk every day.','
Перевод: Я каждый день хожу пешком.'),
('conversation','each','Количество и выбор
Пример: Give one to each person.','
Перевод: Дайте по одному каждому человеку.'),
('conversation','both','Количество и выбор
Пример: Both options are fine.','
Перевод: Оба варианта подходят.'),
('conversation','other','Количество и выбор
Пример: Try the other door.','
Перевод: Попробуйте другую дверь.'),
('conversation','another','Количество и выбор
Пример: Can I have another cup?','
Перевод: Можно ещё одну чашку?'),
('conversation','same','Количество и выбор
Пример: We have the same problem.','
Перевод: У нас одна и та же проблема.'),
('conversation','different','Количество и выбор
Пример: I want a different size.','
Перевод: Я хочу другой размер.'),
('conversation','only','Количество и выбор
Пример: I only have cash.','
Перевод: У меня только наличные.'),
('conversation','also','Количество и выбор
Пример: I also speak Russian.','
Перевод: Я также говорю по‑русски.'),
('conversation','too','Количество и выбор
Пример: This bag is too heavy.','
Перевод: Эта сумка слишком тяжёлая.'),
('conversation','will','Возможность и планы
Пример: I will call you.','
Перевод: Я вам позвоню.'),
('conversation','would','Возможность и планы
Пример: I would like some water.','
Перевод: Я бы хотел воды.'),
('conversation','could','Возможность и планы
Пример: Could you repeat that?','
Перевод: Не могли бы вы повторить?'),
('conversation','should','Возможность и планы
Пример: We should leave now.','
Перевод: Нам следует уйти сейчас.'),
('conversation','must','Возможность и планы
Пример: You must wear a seat belt.','
Перевод: Вы должны пристегнуть ремень безопасности.'),
('conversation','may','Возможность и планы
Пример: May I sit here?','
Перевод: Можно мне здесь сесть?'),
('conversation','might','Возможность и планы
Пример: I might be late.','
Перевод: Я могу опоздать.'),
('conversation','let','Возможность и планы
Пример: Let me help you.','
Перевод: Позвольте мне помочь вам.'),
('conversation','shall','Возможность и планы
Пример: Shall we go?','
Перевод: Пойдём?'),
('conversation','maybe','Возможность и планы
Пример: Maybe tomorrow.','
Перевод: Может быть, завтра.'),
('conversation','probably','Возможность и планы
Пример: I will probably stay home.','
Перевод: Я, вероятно, останусь дома.'),
('conversation','really','Возможность и планы
Пример: I really like it.','
Перевод: Мне это действительно нравится.'),
('conversation','very','Возможность и планы
Пример: It is very good.','
Перевод: Это очень хорошо.'),
('conversation','quite','Возможность и планы
Пример: It is quite warm.','
Перевод: Довольно тепло.'),
('conversation','almost','Возможность и планы
Пример: We are almost there.','
Перевод: Мы почти там.'),
('conversation','even','Возможность и планы
Пример: Even children can do this.','
Перевод: Даже дети могут это сделать.'),
('conversation','ever','Возможность и планы
Пример: Have you ever been here?','
Перевод: Вы когда‑нибудь были здесь?'),
('conversation','perhaps','Возможность и планы
Пример: Perhaps we can meet tomorrow.','
Перевод: Возможно, мы можем встретиться завтра.'),
('conversation','together','Возможность и планы
Пример: Let''s go together.','
Перевод: Пойдём вместе.'),
('conversation','alone','Возможность и планы
Пример: I live alone.','
Перевод: Я живу один.'),
('conversation','start','Возможность и планы
Пример: When do we start?','
Перевод: Когда мы начнём?'),
('conversation','stop','Возможность и планы
Пример: Please stop here.','
Перевод: Пожалуйста, остановитесь здесь.'),
('conversation','finish','Возможность и планы
Пример: I finish work at five.','
Перевод: Я заканчиваю работу в пять.'),
('conversation','plan','Возможность и планы
Пример: What is the plan?','
Перевод: Каков план?'),
('conversation','decide','Возможность и планы
Пример: We need to decide today.','
Перевод: Нам нужно принять решение сегодня.'),
('conversation','choose','Возможность и планы
Пример: Choose a seat.','
Перевод: Выберите место.'),
('conversation','change','Возможность и планы
Пример: Can I change my ticket?','
Перевод: Могу ли я поменять билет?'),
('conversation','move','Возможность и планы
Пример: We moved to a new city.','
Перевод: Мы переехали в новый город.'),
('conversation','turn','Возможность и планы
Пример: Turn left here.','
Перевод: Поверните здесь налево.'),
('conversation','happen','Возможность и планы
Пример: What happened?','
Перевод: Что случилось?'),
('conversation','good','Простые описания
Пример: That is a good idea.','
Перевод: Это хорошая идея.'),
('conversation','bad','Простые описания
Пример: I had a bad day.','
Перевод: У меня был плохой день.'),
('conversation','big','Простые описания
Пример: I need a big bag.','
Перевод: Мне нужна большая сумка.'),
('conversation','small','Простые описания
Пример: We live in a small flat.','
Перевод: Мы живём в небольшой квартире.'),
('conversation','new','Простые описания
Пример: This is my new phone.','
Перевод: Это мой новый телефон.'),
('conversation','old','Простые описания
Пример: My car is old.','
Перевод: Моя машина старая.'),
('conversation','young','Простые описания
Пример: He is still young.','
Перевод: Он всё ещё молод.'),
('conversation','long','Простые описания
Пример: It is a long journey.','
Перевод: Это длинное путешествие.'),
('conversation','short','Простые описания
Пример: Let''s take a short break.','
Перевод: Давайте сделаем короткий перерыв.'),
('conversation','high','Простые описания
Пример: The price is too high.','
Перевод: Цена слишком высока.'),
('conversation','low','Простые описания
Пример: The battery is low.','
Перевод: Батарея разряжена.'),
('conversation','easy','Простые описания
Пример: This is easy to use.','
Перевод: Это легко использовать.'),
('conversation','hard','Простые описания
Пример: Learning takes hard work.','
Перевод: Обучение требует упорного труда.'),
('conversation','simple','Простые описания
Пример: Let me give a simple example.','
Перевод: Позвольте привести простой пример.'),
('conversation','important','Простые описания
Пример: This is important to me.','
Перевод: Это важно для меня.'),
('conversation','right','Простые описания
Пример: You are right.','
Перевод: Вы правы.'),
('conversation','wrong','Простые описания
Пример: I have the wrong address.','
Перевод: У меня неправильный адрес.'),
('conversation','true','Простые описания
Пример: Is that true?','
Перевод: Это правда?'),
('conversation','false','Простые описания
Пример: That information is false.','
Перевод: Эта информация неверна.'),
('conversation','free','Простые описания
Пример: Is this seat free?','
Перевод: Это место свободно?'),
('conversation','busy','Простые описания
Пример: Are you busy tomorrow?','
Перевод: Вы будете заняты завтра?'),
('conversation','ready','Простые описания
Пример: I am ready to go.','
Перевод: Я готов идти.'),
('conversation','sure','Простые описания
Пример: Are you sure?','
Перевод: Вы уверены?'),
('conversation','possible','Простые описания
Пример: Is it possible to change this?','
Перевод: Можно ли это изменить?'),
('conversation','impossible','Простые описания
Пример: That is impossible today.','
Перевод: Это невозможно сегодня.'),
('conversation','open','Простые описания
Пример: Is the shop open?','
Перевод: Магазин открыт?'),
('conversation','closed','Простые описания
Пример: The bank is closed.','
Перевод: Банк закрыт.'),
('conversation','full','Простые описания
Пример: The bus is full.','
Перевод: Автобус полон.'),
('conversation','empty','Простые описания
Пример: This bottle is empty.','
Перевод: Эта бутылка пуста.'),
('conversation','clear','Простые описания
Пример: Your explanation is clear.','
Перевод: Ваше объяснение понятно.'),
('conversation','person','Люди и отношения
Пример: Who is that person?','
Перевод: Кто этот человек?'),
('conversation','people','Люди и отношения
Пример: There are many people here.','
Перевод: Здесь много людей.'),
('conversation','man','Люди и отношения
Пример: Ask that man.','
Перевод: Спросите того мужчину.'),
('conversation','woman','Люди и отношения
Пример: The woman is waiting outside.','
Перевод: Женщина ждёт на улице.'),
('conversation','child','Люди и отношения
Пример: She has one child.','
Перевод: У неё один ребёнок.'),
('conversation','baby','Люди и отношения
Пример: The baby is sleeping.','
Перевод: Малыш спит.'),
('conversation','boy','Люди и отношения
Пример: The boy is my son.','
Перевод: Мальчик — мой сын.'),
('conversation','girl','Люди и отношения
Пример: The girl is my daughter.','
Перевод: Девочка — моя дочь.'),
('conversation','friend','Люди и отношения
Пример: He is a good friend.','
Перевод: Он хороший друг.'),
('conversation','family','Люди и отношения
Пример: My family lives here.','
Перевод: Моя семья живёт здесь.'),
('conversation','mother','Люди и отношения
Пример: My mother is a teacher.','
Перевод: Моя мама — учительница.'),
('conversation','father','Люди и отношения
Пример: My father works nearby.','
Перевод: Мой отец работает рядом.'),
('conversation','parent','Люди и отношения
Пример: A parent must sign this.','
Перевод: Родитель должен подписать это.'),
('conversation','son','Люди и отношения
Пример: My son is at school.','
Перевод: Мой сын в школе.'),
('conversation','daughter','Люди и отношения
Пример: My daughter loves music.','
Перевод: Моя дочь любит музыку.'),
('conversation','brother','Люди и отношения
Пример: I have one brother.','
Перевод: У меня один брат.'),
('conversation','sister','Люди и отношения
Пример: My sister is older than me.','
Перевод: Моя сестра старше меня.'),
('conversation','husband','Люди и отношения
Пример: This is my husband.','
Перевод: Это мой муж.'),
('conversation','wife','Люди и отношения
Пример: My wife speaks English.','
Перевод: Моя жена говорит по-английски.'),
('conversation','partner','Люди и отношения
Пример: I live with my partner.','
Перевод: Я живу со своим партнёром.'),
('conversation','neighbor','Люди и отношения
Пример: Our neighbor is very kind.','
Перевод: Наш сосед очень добрый.'),
('conversation','colleague','Люди и отношения
Пример: Ask my colleague.','
Перевод: Спросите моего коллегу.'),
('conversation','guest','Люди и отношения
Пример: We have a guest today.','
Перевод: У нас сегодня гость.'),
('conversation','customer','Люди и отношения
Пример: The customer needs help.','
Перевод: Клиенту нужна помощь.'),
('conversation','team','Люди и отношения
Пример: We work as a team.','
Перевод: Мы работаем в команде.'),
('conversation','member','Люди и отношения
Пример: She is a team member.','
Перевод: Она — член команды.'),
('conversation','kind','Люди и отношения
Пример: That is very kind of you.','
Перевод: Это очень любезно с вашей стороны.'),
('conversation','friendly','Люди и отношения
Пример: Everyone here is friendly.','
Перевод: Все здесь дружелюбны.'),
('conversation','married','Люди и отношения
Пример: Are you married?','
Перевод: Вы в браке?'),
('conversation','single','Люди и отношения
Пример: I am single.','
Перевод: Я одинок.'),
('conversation','wake','Повседневные действия
Пример: I wake up at seven.','
Перевод: Я просыпаюсь в семь.'),
('conversation','sleep','Повседневные действия
Пример: Did you sleep well?','
Перевод: Вы хорошо спали?'),
('conversation','eat','Повседневные действия
Пример: Let''s eat together.','
Перевод: Давайте поедим вместе.'),
('conversation','drink','Повседневные действия
Пример: Would you like a drink?','
Перевод: Хотите напиток?'),
('conversation','cook','Повседневные действия
Пример: I cook dinner every day.','
Перевод: Я готовлю ужин каждый день.'),
('conversation','wash','Повседневные действия
Пример: Wash your hands.','
Перевод: Помойте руки.'),
('conversation','clean','Повседневные действия
Пример: I need to clean the kitchen.','
Перевод: Мне нужно убрать кухню.'),
('conversation','wear','Повседневные действия
Пример: Wear a warm coat.','
Перевод: Наденьте тёплое пальто.'),
('conversation','dress','Повседневные действия
Пример: I need to get dressed.','
Перевод: Мне нужно одеться.'),
('conversation','walk','Повседневные действия
Пример: Let''s walk home.','
Перевод: Давайте пройдём домой.'),
('conversation','run','Повседневные действия
Пример: I run every morning.','
Перевод: Я бегаю каждое утро.'),
('conversation','sit','Повседневные действия
Пример: Please sit down.','
Перевод: Сядьте, пожалуйста.'),
('conversation','stand','Повседневные действия
Пример: Can you stand here?','
Перевод: Вы можете стоять здесь?'),
('conversation','drive','Повседневные действия
Пример: Can you drive?','
Перевод: Вы умеете водить?'),
('conversation','ride','Повседневные действия
Пример: I ride a bike to work.','
Перевод: Я езжу на велосипеде на работу.'),
('conversation','travel','Повседневные действия
Пример: I like to travel.','
Перевод: Мне нравится путешествовать.'),
('conversation','arrive','Повседневные действия
Пример: We arrive at noon.','
Перевод: Мы прибудем в полдень.'),
('conversation','return','Повседневные действия
Пример: When will you return?','
Перевод: Когда вы вернётесь?'),
('conversation','enter','Повседневные действия
Пример: Please enter your name.','
Перевод: Пожалуйста, введите ваше имя.'),
('conversation','exit','Повседневные действия
Пример: Where is the exit?','
Перевод: Где выход?'),
('conversation','close','Повседневные действия
Пример: Please close the window.','
Перевод: Пожалуйста, закройте окно.'),
('conversation','carry','Повседневные действия
Пример: Can you carry this bag?','
Перевод: Вы можете нести эту сумку?'),
('conversation','hold','Повседневные действия
Пример: Hold my hand.','
Перевод: Держи меня за руку.'),
('conversation','pick','Повседневные действия
Пример: Pick a color.','
Перевод: Выберите цвет.'),
('conversation','drop','Повседневные действия
Пример: Do not drop your phone.','
Перевод: Не бросайте телефон.'),
('conversation','send','Повседневные действия
Пример: Send me a message.','
Перевод: Отправьте мне сообщение.'),
('conversation','receive','Повседневные действия
Пример: Did you receive my email?','
Перевод: Вы получили моё письмо?'),
('conversation','check','Повседневные действия
Пример: Please check the address.','
Перевод: Пожалуйста, проверьте адрес.'),
('conversation','show','Повседневные действия
Пример: Can you show me?','
Перевод: Вы можете показать мне?'),
('conversation','follow','Повседневные действия
Пример: Follow me, please.','
Перевод: Следуйте за мной, пожалуйста.'),
('conversation','mean','Понимать и объяснять
Пример: What does this mean?','
Перевод: Что это значит?'),
('conversation','repeat','Понимать и объяснять
Пример: Could you repeat that?','
Перевод: Не могли бы вы повторить?'),
('conversation','explain','Понимать и объяснять
Пример: Please explain it again.','
Перевод: Пожалуйста, объясните это снова.'),
('conversation','remember','Понимать и объяснять
Пример: I remember your name.','
Перевод: Я помню ваше имя.'),
('conversation','forget','Понимать и объяснять
Пример: Do not forget your keys.','
Перевод: Не забудьте свои ключи.'),
('conversation','agree','Понимать и объяснять
Пример: I agree with you.','
Перевод: Я согласен с вами.'),
('conversation','disagree','Понимать и объяснять
Пример: I disagree, but I understand.','
Перевод: Я не согласен, но понимаю.'),
('conversation','believe','Понимать и объяснять
Пример: I believe you.','
Перевод: Я вам верю.'),
('conversation','hope','Понимать и объяснять
Пример: I hope you feel better.','
Перевод: Надеюсь, вам станет лучше.'),
('conversation','expect','Понимать и объяснять
Пример: What do you expect?','
Перевод: Что вы ожидаете?'),
('conversation','prefer','Понимать и объяснять
Пример: I prefer tea.','
Перевод: Я предпочитаю чай.'),
('conversation','seem','Понимать и объяснять
Пример: You seem tired.','
Перевод: Вы выглядите уставшим.'),
('conversation','notice','Понимать и объяснять
Пример: Did you notice the sign?','
Перевод: Вы заметили знак?'),
('conversation','realize','Понимать и объяснять
Пример: I did not realize it was late.','
Перевод: Я не понял, что уже поздно.'),
('conversation','guess','Понимать и объяснять
Пример: I guess you are right.','
Перевод: Полагаю, вы правы.'),
('conversation','spell','Понимать и объяснять
Пример: How do you spell your name?','
Перевод: Как пишется ваше имя?'),
('conversation','pronounce','Понимать и объяснять
Пример: How do you pronounce this?','
Перевод: Как это произнести?'),
('conversation','translate','Понимать и объяснять
Пример: Can you translate this?','
Перевод: Вы можете перевести это?'),
('conversation','practice','Понимать и объяснять
Пример: I need more practice.','
Перевод: Мне нужно больше практики.'),
('conversation','study','Понимать и объяснять
Пример: I study English every day.','
Перевод: Я изучаю английский каждый день.'),
('conversation','word','Понимать и объяснять
Пример: What does this word mean?','
Перевод: Что значит это слово?'),
('conversation','sentence','Понимать и объяснять
Пример: Write one short sentence.','
Перевод: Напишите одно короткое предложение.'),
('conversation','meaning','Понимать и объяснять
Пример: What is the meaning of this?','
Перевод: Каков смысл этого?'),
('conversation','language','Понимать и объяснять
Пример: Which languages do you speak?','
Перевод: На каких языках вы говорите?'),
('conversation','english','Понимать и объяснять
Пример: I am learning English.','
Перевод: Я изучаю английский.'),
('conversation','russian','Понимать и объяснять
Пример: Do you speak Russian?','
Перевод: Вы говорите по-русски?'),
('conversation','slowly','Понимать и объяснять
Пример: Please speak slowly.','
Перевод: Пожалуйста, говорите медленнее.'),
('conversation','loud','Понимать и объяснять
Пример: The music is too loud.','
Перевод: Музыка слишком громкая.'),
('conversation','quiet','Понимать и объяснять
Пример: Let''s find a quiet place.','
Перевод: Давайте найдём тихое место.'),
('conversation','correct','Понимать и объяснять
Пример: Is this answer correct?','
Перевод: Этот ответ правильный?'),
('conversation','happy','Самочувствие и чувства
Пример: I am happy to help.','
Перевод: Я рад помочь.'),
('conversation','sad','Самочувствие и чувства
Пример: She looks sad.','
Перевод: Она выглядит грустной.'),
('conversation','tired','Самочувствие и чувства
Пример: I am tired today.','
Перевод: Я сегодня устал.'),
('conversation','hungry','Самочувствие и чувства
Пример: Are you hungry?','
Перевод: Вы голодны?'),
('conversation','thirsty','Самочувствие и чувства
Пример: I am thirsty.','
Перевод: Я хочу пить.'),
('conversation','cold','Самочувствие и чувства
Пример: I feel cold.','
Перевод: Мне холодно.'),
('conversation','hot','Самочувствие и чувства
Пример: The soup is hot.','
Перевод: Суп горячий.'),
('conversation','warm','Самочувствие и чувства
Пример: It is warm inside.','
Перевод: Внутри тепло.'),
('conversation','cool','Самочувствие и чувства
Пример: The water is cool.','
Перевод: Вода прохладная.'),
('conversation','sick','Самочувствие и чувства
Пример: I feel sick.','
Перевод: Мне плохо.'),
('conversation','well','Самочувствие и чувства
Пример: I do not feel well.','
Перевод: Я не чувствую себя хорошо.'),
('conversation','better','Самочувствие и чувства
Пример: I feel better now.','
Перевод: Мне сейчас лучше.'),
('conversation','worse','Самочувствие и чувства
Пример: The pain is getting worse.','
Перевод: Боль усиливается.'),
('conversation','fine','Самочувствие и чувства
Пример: I am fine, thanks.','
Перевод: Я в порядке, спасибо.'),
('conversation','ok','Самочувствие и чувства
Пример: Is everything OK?','
Перевод: Всё в порядке?'),
('conversation','afraid','Самочувствие и чувства
Пример: I am afraid of heights.','
Перевод: Я боюсь высоты.'),
('conversation','worried','Самочувствие и чувства
Пример: I am worried about my family.','
Перевод: Я беспокоюсь о своей семье.'),
('conversation','nervous','Самочувствие и чувства
Пример: I feel nervous before interviews.','
Перевод: Я нервничаю перед собеседованиями.'),
('conversation','angry','Самочувствие и чувства
Пример: Why are you angry?','
Перевод: Почему вы злитесь?'),
('conversation','excited','Самочувствие и чувства
Пример: I am excited about the trip.','
Перевод: Я рад предстоящей поездке.'),
('conversation','calm','Самочувствие и чувства
Пример: Try to stay calm.','
Перевод: Постарайтесь сохранять спокойствие.'),
('conversation','surprised','Самочувствие и чувства
Пример: I was surprised to see you.','
Перевод: Я был удивлён, увидев вас.'),
('conversation','bored','Самочувствие и чувства
Пример: I am bored at home.','
Перевод: Мне скучно дома.'),
('conversation','interested','Самочувствие и чувства
Пример: I am interested in this job.','
Перевод: Я заинтересован в этой работе.'),
('conversation','comfortable','Самочувствие и чувства
Пример: This chair is comfortable.','
Перевод: Этот стул удобный.'),
('conversation','uncomfortable','Самочувствие и чувства
Пример: These shoes are uncomfortable.','
Перевод: Эти туфли неудобные.'),
('conversation','safe','Самочувствие и чувства
Пример: Is it safe to walk here?','
Перевод: Здесь безопасно ходить?'),
('conversation','dangerous','Самочувствие и чувства
Пример: This road is dangerous.','
Перевод: Эта дорога опасна.'),
('conversation','careful','Самочувствие и чувства
Пример: Be careful on the stairs.','
Перевод: Будьте осторожны на лестнице.'),
('conversation','strong','Самочувствие и чувства
Пример: I want to get strong.','
Перевод: Я хочу стать сильным.'),
('conversation','food','Еда и напитки: основа
Пример: The food is good.','
Перевод: Еда вкусная.'),
('conversation','water','Еда и напитки: основа
Пример: A glass of water, please.','
Перевод: Стакан воды, пожалуйста.'),
('conversation','tea','Еда и напитки: основа
Пример: Would you like some tea?','
Перевод: Вы хотите чаю?'),
('conversation','coffee','Еда и напитки: основа
Пример: I drink coffee in the morning.','
Перевод: Я пью кофе утром.'),
('conversation','milk','Еда и напитки: основа
Пример: Do you have any milk?','
Перевод: У вас есть молоко?'),
('conversation','bread','Еда и напитки: основа
Пример: I need some bread.','
Перевод: Мне нужен немного хлеба.'),
('conversation','rice','Еда и напитки: основа
Пример: Rice with vegetables, please.','
Перевод: Рис с овощами, пожалуйста.'),
('conversation','pasta','Еда и напитки: основа
Пример: I am cooking pasta.','
Перевод: Я готовлю пасту.'),
('conversation','meat','Еда и напитки: основа
Пример: I do not eat meat.','
Перевод: Я не ем мясо.'),
('conversation','chicken','Еда и напитки: основа
Пример: I would like the chicken.','
Перевод: Я хотел бы курицу.'),
('conversation','fish','Еда и напитки: основа
Пример: Is the fish fresh?','
Перевод: Свежая ли рыба?'),
('conversation','egg','Еда и напитки: основа
Пример: I eat an egg for breakfast.','
Перевод: Я ем яйцо на завтрак.'),
('conversation','cheese','Еда и напитки: основа
Пример: A sandwich with cheese, please.','
Перевод: Сэндвич с сыром, пожалуйста.'),
('conversation','butter','Еда и напитки: основа
Пример: Would you like butter on your bread?','
Перевод: Хотите масло на хлеб?'),
('conversation','salt','Еда и напитки: основа
Пример: Could you pass the salt?','
Перевод: Не могли бы вы передать соль?'),
('conversation','sugar','Еда и напитки: основа
Пример: No sugar, please.','
Перевод: Без сахара, пожалуйста.'),
('conversation','fruit','Еда и напитки: основа
Пример: I eat fruit every day.','
Перевод: Я ем фрукты каждый день.'),
('conversation','apple','Еда и напитки: основа
Пример: Would you like an apple?','
Перевод: Хотите яблоко?'),
('conversation','banana','Еда и напитки: основа
Пример: I packed a banana.','
Перевод: Я упаковал банан.'),
('conversation','orange','Еда и напитки: основа
Пример: A glass of orange juice, please.','
Перевод: Стакан апельсинового сока, пожалуйста.'),
('conversation','vegetable','Еда и напитки: основа
Пример: We need more vegetables.','
Перевод: Нам нужно больше овощей.'),
('conversation','potato','Еда и напитки: основа
Пример: I baked a potato.','
Перевод: Я запек картошку.'),
('conversation','tomato','Еда и напитки: основа
Пример: Add a tomato to the salad.','
Перевод: Добавьте помидор в салат.'),
('conversation','onion','Еда и напитки: основа
Пример: No onion, please.','
Перевод: Без лука, пожалуйста.'),
('conversation','salad','Еда и напитки: основа
Пример: I would like a salad.','
Перевод: Я хотел бы салат.'),
('conversation','soup','Еда и напитки: основа
Пример: The soup is delicious.','
Перевод: Суп вкусный.'),
('conversation','breakfast','Еда и напитки: основа
Пример: What time is breakfast?','
Перевод: Во сколько завтрак?'),
('conversation','lunch','Еда и напитки: основа
Пример: Let''s have lunch.','
Перевод: Давайте пообедаем.'),
('conversation','dinner','Еда и напитки: основа
Пример: Dinner is ready.','
Перевод: Ужин готов.'),
('conversation','meal','Еда и напитки: основа
Пример: Thank you for the meal.','
Перевод: Спасибо за еду.'),
('conversation','home','Дом: основа
Пример: I am going home.','
Перевод: Я иду домой.'),
('conversation','house','Дом: основа
Пример: They bought a house.','
Перевод: Они купили дом.'),
('conversation','apartment','Дом: основа
Пример: I rent an apartment.','
Перевод: Я снимаю квартиру.'),
('conversation','flat','Дом: основа
Пример: The flat has two bedrooms.','
Перевод: В квартире две спальни.'),
('conversation','room','Дом: основа
Пример: Is there room for one more?','
Перевод: Есть место для ещё одного?'),
('conversation','bedroom','Дом: основа
Пример: The bedroom is upstairs.','
Перевод: Спальня наверху.'),
('conversation','bathroom','Дом: основа
Пример: Where is the bathroom?','
Перевод: Где находится ванная?'),
('conversation','kitchen','Дом: основа
Пример: She is in the kitchen.','
Перевод: Она на кухне.'),
('conversation','toilet','Дом: основа
Пример: Where is the toilet?','
Перевод: Где находится туалет?'),
('conversation','door','Дом: основа
Пример: Please shut the door.','
Перевод: Пожалуйста, закройте дверь.'),
('conversation','window','Дом: основа
Пример: Can I open the window?','
Перевод: Можно открыть окно?'),
('conversation','wall','Дом: основа
Пример: The picture is on the wall.','
Перевод: Картина на стене.'),
('conversation','floor','Дом: основа
Пример: We live on the second floor.','
Перевод: Мы живём на втором этаже.'),
('conversation','ceiling','Дом: основа
Пример: The ceiling is high.','
Перевод: Потолок высокий.'),
('conversation','roof','Дом: основа
Пример: The roof is leaking.','
Перевод: Крыша протекает.'),
('conversation','bed','Дом: основа
Пример: I am going to bed.','
Перевод: Я иду спать.'),
('conversation','chair','Дом: основа
Пример: Take a chair.','
Перевод: Возьми стул.'),
('conversation','table','Дом: основа
Пример: Put it on the table.','
Перевод: Положите это на стол.'),
('conversation','sofa','Дом: основа
Пример: Sit on the sofa.','
Перевод: Сядь на диван.'),
('conversation','desk','Дом: основа
Пример: My desk is by the window.','
Перевод: Мой стол стоит у окна.'),
('conversation','light','Дом: основа
Пример: Turn on the light.','
Перевод: Включи свет.'),
('conversation','key','Дом: основа
Пример: I lost my key.','
Перевод: Я потерял ключ.'),
('conversation','lock','Дом: основа
Пример: Lock the door.','
Перевод: Запри дверь.'),
('conversation','bag','Дом: основа
Пример: This is my bag.','
Перевод: Это моя сумка.'),
('conversation','box','Дом: основа
Пример: What is in the box?','
Перевод: Что в коробке?'),
('conversation','bottle','Дом: основа
Пример: A bottle of water, please.','
Перевод: Бутылку воды, пожалуйста.'),
('conversation','cup','Дом: основа
Пример: Would you like a cup of tea?','
Перевод: Хотите чашку чая?'),
('conversation','glass','Дом: основа
Пример: I broke a glass.','
Перевод: Я разбил стакан.'),
('conversation','plate','Дом: основа
Пример: Put it on a plate.','
Перевод: Положи это на тарелку.'),
('conversation','spoon','Дом: основа
Пример: I need a spoon.','
Перевод: Мне нужна ложка.'),
('conversation','place','Город и первые поездки
Пример: This is a nice place.','
Перевод: Это хорошее место.'),
('conversation','city','Город и первые поездки
Пример: Which city do you live in?','
Перевод: В каком городе вы живёте?'),
('conversation','town','Город и первые поездки
Пример: I live in a small town.','
Перевод: Я живу в небольшом городе.'),
('conversation','street','Город и первые поездки
Пример: What street is it on?','
Перевод: На какой улице это находится?'),
('conversation','road','Город и первые поездки
Пример: Cross the road carefully.','
Перевод: Перейдите дорогу осторожно.'),
('conversation','address','Город и первые поездки
Пример: What is your address?','
Перевод: Какой у вас адрес?'),
('conversation','station','Город и первые поездки
Пример: Where is the train station?','
Перевод: Где находится вокзал?'),
('conversation','bus','Город и первые поездки
Пример: I take the bus to work.','
Перевод: Я езжу на автобусе на работу.'),
('conversation','train','Город и первые поездки
Пример: The train is late.','
Перевод: Поезд опаздывает.'),
('conversation','car','Город и первые поездки
Пример: I do not have a car.','
Перевод: У меня нет машины.'),
('conversation','taxi','Город и первые поездки
Пример: Could you call a taxi?','
Перевод: Не могли бы вы вызвать такси?'),
('conversation','airport','Город и первые поездки
Пример: How far is the airport?','
Перевод: Как далеко находится аэропорт?'),
('conversation','plane','Город и первые поездки
Пример: Our plane leaves at six.','
Перевод: Наш рейс вылетает в шесть.'),
('conversation','ticket','Город и первые поездки
Пример: I need a return ticket.','
Перевод: Мне нужен билет туда и обратно.'),
('conversation','passport','Город и первые поездки
Пример: Here is my passport.','
Перевод: Вот мой паспорт.'),
('conversation','hotel','Город и первые поездки
Пример: I booked a hotel.','
Перевод: Я забронировал отель.'),
('conversation','shop','Город и первые поездки
Пример: The shop is closed.','
Перевод: Магазин закрыт.'),
('conversation','store','Город и первые поездки
Пример: Is there a store nearby?','
Перевод: Есть ли рядом магазин?'),
('conversation','market','Город и первые поездки
Пример: Let''s go to the market.','
Перевод: Пойдём на рынок.'),
('conversation','bank','Город и первые поездки
Пример: The bank opens at nine.','
Перевод: Банк открывается в девять.'),
('conversation','hospital','Город и первые поездки
Пример: Take me to the hospital.','
Перевод: Отвезите меня в больницу.'),
('conversation','pharmacy','Город и первые поездки
Пример: Where is the nearest pharmacy?','
Перевод: Где находится ближайшая аптека?'),
('conversation','park','Город и первые поездки
Пример: We walk in the park.','
Перевод: Мы гуляем в парке.'),
('conversation','restaurant','Город и первые поездки
Пример: Can you recommend a restaurant?','
Перевод: Можете порекомендовать ресторан?'),
('conversation','cafe','Город и первые поездки
Пример: Let''s meet at the cafe.','
Перевод: Встретимся в кафе.'),
('conversation','left','Город и первые поездки
Пример: Turn left at the lights.','
Перевод: Поверните налево у светофора.'),
('conversation','straight','Город и первые поездки
Пример: Go straight ahead.','
Перевод: Идите прямо.'),
('conversation','far','Город и первые поездки
Пример: Is it far from here?','
Перевод: Это далеко отсюда?'),
('conversation','map','Город и первые поездки
Пример: Can you show me on the map?','
Перевод: Можете показать на карте?'),
('conversation','way','Город и первые поездки
Пример: Which way is the station?','
Перевод: Как пройти к станции?'),
('conversation','money','Покупки и деньги
Пример: I need to save money.','
Перевод: Мне нужно экономить деньги.'),
('conversation','price','Покупки и деньги
Пример: What is the price?','
Перевод: Сколько стоит?'),
('conversation','cost','Покупки и деньги
Пример: How much does it cost?','
Перевод: Сколько это стоит?'),
('conversation','cheap','Покупки и деньги
Пример: Is there a cheaper option?','
Перевод: Есть ли более дешёвая альтернатива?'),
('conversation','expensive','Покупки и деньги
Пример: This is too expensive.','
Перевод: Это слишком дорого.'),
('conversation','cash','Покупки и деньги
Пример: Do you accept cash?','
Перевод: Вы принимаете наличные?'),
('conversation','card','Покупки и деньги
Пример: Can I pay by card?','
Перевод: Можно оплатить картой?'),
('conversation','coin','Покупки и деньги
Пример: Do you have a coin?','
Перевод: У вас есть монета?'),
('conversation','receipt','Покупки и деньги
Пример: Can I have a receipt?','
Перевод: Можно получить чек?'),
('conversation','bill','Покупки и деньги
Пример: The bill, please.','
Перевод: Счёт, пожалуйста.'),
('conversation','discount','Покупки и деньги
Пример: Is there a discount?','
Перевод: Есть ли скидка?'),
('conversation','sale','Покупки и деньги
Пример: These shoes are on sale.','
Перевод: Эти туфли сейчас в распродаже.'),
('conversation','size','Покупки и деньги
Пример: Do you have my size?','
Перевод: Есть ли у вас мой размер?'),
('conversation','color','Покупки и деньги
Пример: What color do you want?','
Перевод: Какой цвет вы хотите?'),
('conversation','fit','Покупки и деньги
Пример: These shoes do not fit.','
Перевод: Эти туфли не подходят.'),
('conversation','pair','Покупки и деньги
Пример: I need a pair of socks.','
Перевод: Мне нужна пара носков.'),
('conversation','order','Покупки и деньги
Пример: I would like to order now.','
Перевод: Я хотел бы сделать заказ сейчас.'),
('conversation','sell','Покупки и деньги
Пример: Do you sell phone chargers?','
Перевод: Вы продаёте зарядные устройства для телефонов?'),
('conversation','spend','Покупки и деньги
Пример: I spend less on food now.','
Перевод: Сейчас я трачу меньше на еду.'),
('conversation','save','Покупки и деньги
Пример: I am saving for a car.','
Перевод: Я коплю на машину.'),
('conversation','borrow','Покупки и деньги
Пример: Can I borrow your pen?','
Перевод: Можно одолжить вашу ручку?'),
('conversation','lend','Покупки и деньги
Пример: Can you lend me some money?','
Перевод: Можно одолжить немного денег?'),
('conversation','afford','Покупки и деньги
Пример: I cannot afford it.','
Перевод: Я не могу себе это позволить.'),
('conversation','worth','Покупки и деньги
Пример: Is it worth the price?','
Перевод: Стоит ли эта цена?'),
('conversation','refund','Покупки и деньги
Пример: I would like a refund.','
Перевод: Я хотел бы вернуть деньги.'),
('conversation','exchange','Покупки и деньги
Пример: Can I exchange this shirt?','
Перевод: Можно обменять эту рубашку?'),
('conversation','amount','Покупки и деньги
Пример: Check the amount, please.','
Перевод: Проверьте, пожалуйста, сумму.'),
('conversation','total','Покупки и деньги
Пример: What is the total?','
Перевод: Какова общая сумма?'),
('conversation','extra','Покупки и деньги
Пример: Does delivery cost extra?','
Перевод: Доставка стоит дополнительно?'),
('conversation','clothes','Одежда и вещи с собой
Пример: I need warm clothes.','
Перевод: Мне нужна тёплая одежда.'),
('conversation','shirt','Одежда и вещи с собой
Пример: This shirt is too small.','
Перевод: Эта рубашка слишком маленькая.'),
('conversation','t-shirt','Одежда и вещи с собой
Пример: I wear a T-shirt at home.','
Перевод: Я ношу футболку дома.'),
('conversation','trousers','Одежда и вещи с собой
Пример: These trousers are too long.','
Перевод: Эти брюки слишком длинные.'),
('conversation','pants','Одежда и вещи с собой
Пример: I need new pants for work.','
Перевод: Мне нужны новые брюки для работы.'),
('conversation','jeans','Одежда и вещи с собой
Пример: I usually wear jeans.','
Перевод: Я обычно ношу джинсы.'),
('conversation','shorts','Одежда и вещи с собой
Пример: It is warm enough for shorts.','
Перевод: Достаточно тепло, чтобы носить шорты.'),
('conversation','skirt','Одежда и вещи с собой
Пример: She bought a blue skirt.','
Перевод: Она купила синюю юбку.'),
('conversation','jacket','Одежда и вещи с собой
Пример: Take your jacket.','
Перевод: Возьмите свою куртку.'),
('conversation','coat','Одежда и вещи с собой
Пример: You need a warm coat.','
Перевод: Тебе нужен тёплый плащ.'),
('conversation','sweater','Одежда и вещи с собой
Пример: This sweater is soft.','
Перевод: Этот свитер мягкий.'),
('conversation','shoe','Одежда и вещи с собой
Пример: There is a stone in my shoe.','
Перевод: В моей обуви камень.'),
('conversation','boot','Одежда и вещи с собой
Пример: My boots are wet.','
Перевод: Мои ботинки мокрые.'),
('conversation','sock','Одежда и вещи с собой
Пример: I cannot find my other sock.','
Перевод: Я не могу найти другую носок.'),
('conversation','hat','Одежда и вещи с собой
Пример: Wear a hat in the sun.','
Перевод: Носи шляпу на солнце.'),
('conversation','cap','Одежда и вещи с собой
Пример: I left my cap at home.','
Перевод: Я оставил свою кепку дома.'),
('conversation','scarf','Одежда и вещи с собой
Пример: This scarf is warm.','
Перевод: Этот шарф тёплый.'),
('conversation','glove','Одежда и вещи с собой
Пример: I lost a glove.','
Перевод: Я потерял перчатку.'),
('conversation','belt','Одежда и вещи с собой
Пример: I need a belt.','
Перевод: Мне нужен ремень.'),
('conversation','pocket','Одежда и вещи с собой
Пример: My phone is in my pocket.','
Перевод: Мой телефон в кармане.'),
('conversation','wallet','Одежда и вещи с собой
Пример: I lost my wallet.','
Перевод: Я потерял свой кошелёк.'),
('conversation','purse','Одежда и вещи с собой
Пример: She put her keys in her purse.','
Перевод: Она положила ключи в сумочку.'),
('conversation','watch','Одежда и вещи с собой
Пример: My watch is slow.','
Перевод: Мои часы идут медленно.'),
('conversation','ring','Одежда и вещи с собой
Пример: I lost my wedding ring.','
Перевод: Я потерял обручальное кольцо.'),
('conversation','glasses','Одежда и вещи с собой
Пример: I need my glasses to read.','
Перевод: Мне нужны очки, чтобы читать.'),
('conversation','umbrella','Одежда и вещи с собой
Пример: Take an umbrella.','
Перевод: Возьми зонтик.'),
('conversation','backpack','Одежда и вещи с собой
Пример: My backpack is heavy.','
Перевод: Мой рюкзак тяжёлый.'),
('conversation','suitcase','Одежда и вещи с собой
Пример: This suitcase is mine.','
Перевод: Этот чемодан мой.'),
('conversation','phone','Одежда и вещи с собой
Пример: My phone is broken.','
Перевод: Мой телефон сломан.'),
('conversation','charger','Одежда и вещи с собой
Пример: Can I borrow a charger?','
Перевод: Можно одолжить зарядное устройство?'),
('conversation','morning','Календарь и распорядок
Пример: I work in the morning.','
Перевод: Я работаю утром.'),
('conversation','afternoon','Календарь и распорядок
Пример: See you this afternoon.','
Перевод: Увидимся сегодня во второй половине дня.'),
('conversation','evening','Календарь и распорядок
Пример: I cook in the evening.','
Перевод: Я готовлю вечером.'),
('conversation','night','Календарь и распорядок
Пример: I work at night.','
Перевод: Я работаю ночью.'),
('conversation','noon','Календарь и распорядок
Пример: Let''s meet at noon.','
Перевод: Давай встретимся в полдень.'),
('conversation','midnight','Календарь и распорядок
Пример: The last bus leaves at midnight.','
Перевод: Последний автобус уходит в полночь.'),
('conversation','minute','Календарь и распорядок
Пример: Wait a minute.','
Перевод: Подожди минуту.'),
('conversation','second','Календарь и распорядок
Пример: Give me a second.','
Перевод: Дай секунду.'),
('conversation','weekend','Календарь и распорядок
Пример: What are you doing this weekend?','
Перевод: Что ты будешь делать в эти выходные?'),
('conversation','monday','Календарь и распорядок
Пример: See you on Monday.','
Перевод: Увидимся в понедельник.'),
('conversation','tuesday','Календарь и распорядок
Пример: I am free on Tuesday.','
Перевод: Я свободен во вторник.'),
('conversation','wednesday','Календарь и распорядок
Пример: The meeting is on Wednesday.','
Перевод: Собрание в среду.'),
('conversation','thursday','Календарь и распорядок
Пример: I work on Thursday.','
Перевод: Я работаю в четверг.'),
('conversation','friday','Календарь и распорядок
Пример: We leave on Friday.','
Перевод: Мы уезжаем в пятницу.'),
('conversation','saturday','Календарь и распорядок
Пример: The shop is busy on Saturday.','
Перевод: Магазин занят в субботу.'),
('conversation','sunday','Календарь и распорядок
Пример: I rest on Sunday.','
Перевод: Я отдыхаю в воскресенье.'),
('conversation','january','Календарь и распорядок
Пример: It is cold in January.','
Перевод: В январе холодно.'),
('conversation','february','Календарь и распорядок
Пример: My birthday is in February.','
Перевод: Мой день рождения в феврале.'),
('conversation','march','Календарь и распорядок
Пример: We moved here in March.','
Перевод: Мы переехали сюда в марте.'),
('conversation','april','Календарь и распорядок
Пример: The course starts in April.','
Перевод: Курс начинается в апреле.'),
('conversation','june','Календарь и распорядок
Пример: School finishes in June.','
Перевод: Школа заканчивается в июне.'),
('conversation','july','Календарь и распорядок
Пример: It is hot in July.','
Перевод: В июле жарко.'),
('conversation','august','Календарь и распорядок
Пример: We travel in August.','
Перевод: Мы путешествуем в августе.'),
('conversation','september','Календарь и распорядок
Пример: Classes start in September.','
Перевод: Занятия начинаются в сентябре.'),
('conversation','october','Календарь и распорядок
Пример: We met in October.','
Перевод: Мы встретились в октябре.'),
('conversation','november','Календарь и распорядок
Пример: It rains a lot in November.','
Перевод: В ноябре часто идут дожди.'),
('conversation','december','Календарь и распорядок
Пример: I visit my family in December.','
Перевод: Я навещаю свою семью в декабре.'),
('conversation','date','Календарь и распорядок
Пример: What is the date today?','
Перевод: Какое сегодня число?'),
('conversation','calendar','Календарь и распорядок
Пример: Let me check my calendar.','
Перевод: Позвольте проверить мой календарь.'),
('conversation','zero','Числа в жизни
Пример: The temperature is below zero.','
Перевод: Температура ниже нуля.'),
('conversation','eleven','Числа в жизни
Пример: The bus leaves at eleven.','
Перевод: Автобус отправляется в одиннадцать.'),
('conversation','twelve','Числа в жизни
Пример: There are twelve months in a year.','
Перевод: В году двенадцать месяцев.'),
('conversation','thirteen','Числа в жизни
Пример: My daughter is thirteen.','
Перевод: Моей дочери тринадцать лет.'),
('conversation','fourteen','Числа в жизни
Пример: There are fourteen people here.','
Перевод: Здесь четырнадцать человек.'),
('conversation','fifteen','Числа в жизни
Пример: It takes fifteen minutes.','
Перевод: Это занимает пятнадцать минут.'),
('conversation','sixteen','Числа в жизни
Пример: My son is sixteen.','
Перевод: Моему сыну шестнадцать лет.'),
('conversation','seventeen','Числа в жизни
Пример: Room seventeen is upstairs.','
Перевод: Комната семнадцать находится наверху.'),
('conversation','eighteen','Числа в жизни
Пример: You must be eighteen to apply.','
Перевод: Для подачи заявки вам должно быть восемнадцать лет.'),
('conversation','nineteen','Числа в жизни
Пример: The ticket costs nineteen euros.','
Перевод: Билет стоит девятнадцать евро.'),
('conversation','twenty','Числа в жизни
Пример: I will be there in twenty minutes.','
Перевод: Я буду там через двадцать минут.'),
('conversation','thirty','Числа в жизни
Пример: It takes thirty minutes.','
Перевод: Это занимает тридцать минут.'),
('conversation','forty','Числа в жизни
Пример: The bag costs forty euros.','
Перевод: Сумка стоит сорок евро.'),
('conversation','fifty','Числа в жизни
Пример: There are fifty seats.','
Перевод: Есть пятьдесят мест.'),
('conversation','sixty','Числа в жизни
Пример: There are sixty minutes in an hour.','
Перевод: В часе шестьдесят минут.'),
('conversation','seventy','Числа в жизни
Пример: My father is seventy.','
Перевод: Моему отцу семьдесят лет.'),
('conversation','eighty','Числа в жизни
Пример: The room costs eighty euros.','
Перевод: Комната стоит восемьдесят евро.'),
('conversation','ninety','Числа в жизни
Пример: The film lasts ninety minutes.','
Перевод: Фильм длится девяносто минут.'),
('conversation','hundred','Числа в жизни
Пример: It costs a hundred euros.','
Перевод: Это стоит сто евро.'),
('conversation','thousand','Числа в жизни
Пример: The car costs five thousand euros.','
Перевод: Автомобиль стоит пять тысяч евро.'),
('conversation','million','Числа в жизни
Пример: A million people live here.','
Перевод: Здесь живёт миллион человек.'),
('conversation','first','Числа в жизни
Пример: This is my first visit.','
Перевод: Это мой первый визит.'),
('conversation','third','Числа в жизни
Пример: Take the third street on the right.','
Перевод: Возьмите третью улицу направо.'),
('conversation','fourth','Числа в жизни
Пример: I live on the fourth floor.','
Перевод: Я живу на четвертом этаже.'),
('conversation','last','Числа в жизни
Пример: I missed the last bus.','
Перевод: Я пропустил последний автобус.'),
('conversation','next','Числа в жизни
Пример: Get off at the next stop.','
Перевод: Выйдите на следующей остановке.'),
('conversation','half','Числа в жизни
Пример: I''ll be there in half an hour.','
Перевод: Я буду там через полчаса.'),
('conversation','quarter','Числа в жизни
Пример: It is a quarter past six.','
Перевод: Сейчас четверть седьмого.'),
('conversation','double','Числа в жизни
Пример: I booked a double room.','
Перевод: Я забронировал двухместный номер.'),
('conversation','number','Числа в жизни
Пример: What is your phone number?','
Перевод: Какой у вас номер телефона?'),
('conversation','menu','В кафе и на кухне
Пример: Can I see the menu?','
Перевод: Можно посмотреть меню?'),
('conversation','waiter','В кафе и на кухне
Пример: The waiter brought the bill.','
Перевод: Официант принес счёт.'),
('conversation','waitress','В кафе и на кухне
Пример: Ask the waitress for water.','
Перевод: Попросите официантку воды.'),
('conversation','serve','В кафе и на кухне
Пример: Do you serve breakfast?','
Перевод: Вы подаёте завтрак?'),
('conversation','portion','В кафе и на кухне
Пример: A small portion, please.','
Перевод: Небольшая порция, пожалуйста.'),
('conversation','taste','В кафе и на кухне
Пример: This tastes good.','
Перевод: Это вкусно.'),
('conversation','delicious','В кафе и на кухне
Пример: The food is delicious.','
Перевод: Еда восхитительная.'),
('conversation','fresh','В кафе и на кухне
Пример: Is this bread fresh?','
Перевод: Этот хлеб свежий?'),
('conversation','spicy','В кафе и на кухне
Пример: Is it very spicy?','
Перевод: Он очень острый?'),
('conversation','sweet','В кафе и на кухне
Пример: This tea is too sweet.','
Перевод: Этот чай слишком сладкий.'),
('conversation','sour','В кафе и на кухне
Пример: The lemon is sour.','
Перевод: Лимон кислый.'),
('conversation','bitter','В кафе и на кухне
Пример: This coffee tastes bitter.','
Перевод: Этот кофе горький на вкус.'),
('conversation','salty','В кафе и на кухне
Пример: The soup is too salty.','
Перевод: Суп слишком солёный.'),
('conversation','raw','В кафе и на кухне
Пример: Do not eat raw chicken.','
Перевод: Не ешьте сырую курицу.'),
('conversation','ripe','В кафе и на кухне
Пример: These bananas are ripe.','
Перевод: Эти бананы спелые.'),
('conversation','beef','В кафе и на кухне
Пример: I do not eat beef.','
Перевод: Я не ем говядину.'),
('conversation','pork','В кафе и на кухне
Пример: Does this contain pork?','
Перевод: Содержит ли это свинину?'),
('conversation','lamb','В кафе и на кухне
Пример: I ordered lamb with rice.','
Перевод: Я заказал ягнёнка с рисом.'),
('conversation','seafood','В кафе и на кухне
Пример: I am allergic to seafood.','
Перевод: У меня аллергия на морепродукты.'),
('conversation','sandwich','В кафе и на кухне
Пример: A cheese sandwich, please.','
Перевод: Сэндвич с сыром, пожалуйста.'),
('conversation','pizza','В кафе и на кухне
Пример: Let''s order a pizza.','
Перевод: Давайте закажем пиццу.'),
('conversation','cake','В кафе и на кухне
Пример: Would you like some cake?','
Перевод: Хотите кусочек торта?'),
('conversation','cookie','В кафе и на кухне
Пример: Have a cookie.','
Перевод: Возьмите печенье.'),
('conversation','chocolate','В кафе и на кухне
Пример: I like dark chocolate.','
Перевод: Мне нравится тёмный шоколад.'),
('conversation','ice','В кафе и на кухне
Пример: No ice, please.','
Перевод: Без льда, пожалуйста.'),
('conversation','cream','В кафе и на кухне
Пример: Would you like cream in your coffee?','
Перевод: Хотите сливки в кофе?'),
('conversation','juice','В кафе и на кухне
Пример: Apple juice, please.','
Перевод: Яблочный сок, пожалуйста.'),
('conversation','beer','В кафе и на кухне
Пример: A small beer, please.','
Перевод: Небольшое пиво, пожалуйста.'),
('conversation','wine','В кафе и на кухне
Пример: A glass of red wine, please.','
Перевод: Стакан красного вина, пожалуйста.'),
('conversation','alcohol','В кафе и на кухне
Пример: I do not drink alcohol.','
Перевод: Я не пью алкоголь.'),
('conversation','carrot','Продукты и готовка
Пример: Cut the carrot into small pieces.','
Перевод: Нарежьте морковь мелкими кусочками.'),
('conversation','cucumber','Продукты и готовка
Пример: Add cucumber to the salad.','
Перевод: Добавьте огурец в салат.'),
('conversation','pepper','Продукты и готовка
Пример: Pass the pepper, please.','
Перевод: Передайте, пожалуйста, перец.'),
('conversation','cabbage','Продукты и готовка
Пример: I made cabbage soup.','
Перевод: Я приготовил капустный суп.'),
('conversation','garlic','Продукты и готовка
Пример: This sauce contains garlic.','
Перевод: В этом соусе есть чеснок.'),
('conversation','bean','Продукты и готовка
Пример: I often cook rice and beans.','
Перевод: Я часто готовлю рис с бобами.'),
('conversation','pea','Продукты и готовка
Пример: Would you like some peas?','
Перевод: Хотите горох?'),
('conversation','mushroom','Продукты и готовка
Пример: I made mushroom soup.','
Перевод: Я приготовил грибной суп.'),
('conversation','lettuce','Продукты и готовка
Пример: Put some lettuce in the sandwich.','
Перевод: Положите немного салата в сэндвич.'),
('conversation','lemon','Продукты и готовка
Пример: Tea with lemon, please.','
Перевод: Чай с лимоном, пожалуйста.'),
('conversation','grape','Продукты и готовка
Пример: These grapes are sweet.','
Перевод: Эти виноградные ягоды сладкие.'),
('conversation','strawberry','Продукты и готовка
Пример: Would you like some strawberries?','
Перевод: Хотите немного клубники?'),
('conversation','pear','Продукты и готовка
Пример: This pear is ripe.','
Перевод: Эта груша спелая.'),
('conversation','peach','Продукты и готовка
Пример: I bought a peach.','
Перевод: Я купил персик.'),
('conversation','yogurt','Продукты и готовка
Пример: I eat yogurt for breakfast.','
Перевод: Я ем йогурт на завтрак.'),
('conversation','oil','Продукты и готовка
Пример: Heat some oil in a pan.','
Перевод: Разогрейте немного масла в сковороде.'),
('conversation','flour','Продукты и готовка
Пример: We need flour to make bread.','
Перевод: Нужна мука, чтобы испечь хлеб.'),
('conversation','honey','Продукты и готовка
Пример: I put honey in my tea.','
Перевод: Я добавляю мёд в чай.'),
('conversation','cereal','Продукты и готовка
Пример: I have cereal with milk.','
Перевод: У меня хлопья с молоком.'),
('conversation','nut','Продукты и готовка
Пример: Does this contain nuts?','
Перевод: Содержит ли это орехи?'),
('conversation','sauce','Продукты и готовка
Пример: Could I have the sauce separately?','
Перевод: Можно ли подать соус отдельно?'),
('conversation','recipe','Продукты и готовка
Пример: Can you send me the recipe?','
Перевод: Можете прислать мне рецепт?'),
('conversation','ingredient','Продукты и готовка
Пример: What ingredients do I need?','
Перевод: Какие ингредиенты мне нужны?'),
('conversation','boil','Продукты и готовка
Пример: Boil the water first.','
Перевод: Сначала вскипятите воду.'),
('conversation','fry','Продукты и готовка
Пример: Fry the onions in a little oil.','
Перевод: Обжарьте лук на небольшом количестве масла.'),
('conversation','bake','Продукты и готовка
Пример: I bake bread at home.','
Перевод: Я пеку хлеб дома.'),
('conversation','cut','Продукты и готовка
Пример: Cut the bread, please.','
Перевод: Пожалуйста, нарежьте хлеб.'),
('conversation','mix','Продукты и готовка
Пример: Mix the eggs with the milk.','
Перевод: Смешайте яйца с молоком.'),
('conversation','add','Продукты и готовка
Пример: Add a little salt.','
Перевод: Добавьте немного соли.'),
('conversation','heat','Продукты и готовка
Пример: Heat the soup before serving.','
Перевод: Разогрейте суп перед подачей.'),
('conversation','fridge','Домашние дела
Пример: Put the milk in the fridge.','
Перевод: Положите молоко в холодильник.'),
('conversation','freezer','Домашние дела
Пример: The fish is in the freezer.','
Перевод: Рыба находится в морозильнике.'),
('conversation','oven','Домашние дела
Пример: The oven is hot.','
Перевод: Духовка разогрета.'),
('conversation','stove','Домашние дела
Пример: Turn off the stove.','
Перевод: Выключите плиту.'),
('conversation','microwave','Домашние дела
Пример: Heat it in the microwave.','
Перевод: Разогрейте это в микроволновке.'),
('conversation','kettle','Домашние дела
Пример: Fill the kettle with water.','
Перевод: Налейте в чайник воду.'),
('conversation','sink','Домашние дела
Пример: The sink is blocked.','
Перевод: Раковина заблокирована.'),
('conversation','tap','Домашние дела
Пример: Turn off the tap.','
Перевод: Закройте кран.'),
('conversation','faucet','Домашние дела
Пример: The faucet is leaking.','
Перевод: Кран протекает.'),
('conversation','shower','Домашние дела
Пример: I need a shower.','
Перевод: Мне нужен душ.'),
('conversation','bath','Домашние дела
Пример: I am taking a bath.','
Перевод: Я принимаю ванну.'),
('conversation','towel','Домашние дела
Пример: Can I have a clean towel?','
Перевод: Можно мне чистое полотенце?'),
('conversation','soap','Домашние дела
Пример: Wash your hands with soap.','
Перевод: Мойте руки с мылом.'),
('conversation','shampoo','Домашние дела
Пример: We need more shampoo.','
Перевод: Нужен ещё шампунь.'),
('conversation','toothbrush','Домашние дела
Пример: I forgot my toothbrush.','
Перевод: Я забыл зубную щётку.'),
('conversation','toothpaste','Домашние дела
Пример: Where is the toothpaste?','
Перевод: Где зубная паста?'),
('conversation','mirror','Домашние дела
Пример: Look in the mirror.','
Перевод: Посмотрите в зеркало.'),
('conversation','brush','Домашние дела
Пример: I brush my teeth twice a day.','
Перевод: Я чищу зубы дважды в день.'),
('conversation','comb','Домашние дела
Пример: Can I borrow a comb?','
Перевод: Можно я возьму расчёску?'),
('conversation','blanket','Домашние дела
Пример: Could I have another blanket?','
Перевод: Можно мне ещё одно одеяло?'),
('conversation','pillow','Домашние дела
Пример: This pillow is too soft.','
Перевод: Эта подушка слишком мягкая.'),
('conversation','sheet','Домашние дела
Пример: We need clean sheets.','
Перевод: Нужны чистые простыни.'),
('conversation','carpet','Домашние дела
Пример: The carpet needs cleaning.','
Перевод: Ковёр нужно очистить.'),
('conversation','curtain','Домашние дела
Пример: Please close the curtains.','
Перевод: Пожалуйста, закройте шторы.'),
('conversation','shelf','Домашние дела
Пример: Put it on the shelf.','
Перевод: Положите это на полку.'),
('conversation','drawer','Домашние дела
Пример: The keys are in the drawer.','
Перевод: Ключи в ящике.'),
('conversation','cupboard','Домашние дела
Пример: The cups are in the cupboard.','
Перевод: Стаканы в шкафу.'),
('conversation','furniture','Домашние дела
Пример: The room has little furniture.','
Перевод: В комнате мало мебели.'),
('conversation','rubbish','Домашние дела
Пример: Take out the rubbish.','
Перевод: Выведите мусор.'),
('conversation','trash','Домашние дела
Пример: Put it in the trash.','
Перевод: Положите это в мусорное ведро.'),
('conversation','job','Работа и учёба
Пример: I am looking for a job.','
Перевод: Я ищу работу.'),
('conversation','office','Работа и учёба
Пример: I work in an office.','
Перевод: Я работаю в офисе.'),
('conversation','company','Работа и учёба
Пример: Which company do you work for?','
Перевод: В какой компании вы работаете?'),
('conversation','boss','Работа и учёба
Пример: I need to speak to my boss.','
Перевод: Мне нужно поговорить с моим начальником.'),
('conversation','manager','Работа и учёба
Пример: Can I speak to the manager?','
Перевод: Можно поговорить с менеджером?'),
('conversation','worker','Работа и учёба
Пример: The workers need a break.','
Перевод: Работникам нужен перерыв.'),
('conversation','employee','Работа и учёба
Пример: She is a new employee.','
Перевод: Она новый сотрудник.'),
('conversation','employer','Работа и учёба
Пример: My employer provides training.','
Перевод: Мой работодатель предоставляет обучение.'),
('conversation','salary','Работа и учёба
Пример: What is the salary?','
Перевод: Какая зарплата?'),
('conversation','meeting','Работа и учёба
Пример: I have a meeting at ten.','
Перевод: У меня встреча в десять.'),
('conversation','break','Работа и учёба
Пример: Let''s take a break.','
Перевод: Давайте сделаем перерыв.'),
('conversation','shift','Работа и учёба
Пример: My shift starts at six.','
Перевод: Моя смена начинается в шесть.'),
('conversation','task','Работа и учёба
Пример: I have one more task.','
Перевод: У меня ещё одно задание.'),
('conversation','problem','Работа и учёба
Пример: We have a problem.','
Перевод: У нас проблема.'),
('conversation','solution','Работа и учёба
Пример: Let''s find a solution.','
Перевод: Давайте найдём решение.'),
('conversation','idea','Работа и учёба
Пример: I have an idea.','
Перевод: У меня есть идея.'),
('conversation','reason','Работа и учёба
Пример: What is the reason?','
Перевод: В чём причина?'),
('conversation','result','Работа и учёба
Пример: When will we get the results?','
Перевод: Когда мы получим результаты?'),
('conversation','example','Работа и учёба
Пример: Can you give me an example?','
Перевод: Можете привести пример?'),
('conversation','experience','Работа и учёба
Пример: I have two years of experience.','
Перевод: У меня два года опыта.'),
('conversation','skill','Работа и учёба
Пример: I want to improve my skills.','
Перевод: Я хочу улучшить свои навыки.'),
('conversation','school','Работа и учёба
Пример: My children go to school here.','
Перевод: Мои дети учатся в школе здесь.'),
('conversation','student','Работа и учёба
Пример: I am a student.','
Перевод: Я студент.'),
('conversation','teacher','Работа и учёба
Пример: Ask your teacher.','
Перевод: Спросите у вашего учителя.'),
('conversation','class','Работа и учёба
Пример: The class starts at nine.','
Перевод: Занятие начинается в девять.'),
('conversation','lesson','Работа и учёба
Пример: I have an English lesson today.','
Перевод: У меня сегодня урок английского.'),
('conversation','course','Работа и учёба
Пример: I want to take this course.','
Перевод: Я хочу пройти этот курс.'),
('conversation','book','Работа и учёба
Пример: I am reading a book.','
Перевод: Я читаю книгу.'),
('conversation','page','Работа и учёба
Пример: Open the book at page ten.','
Перевод: Откройте книгу на странице десять.'),
('conversation','pen','Работа и учёба
Пример: Can I borrow a pen?','
Перевод: Можно одолжить ручку?'),
('conversation','message','Связь и повседневные технологии
Пример: Send me a message.','
Перевод: Отправьте мне сообщение.'),
('conversation','email','Связь и повседневные технологии
Пример: Check your email.','
Перевод: Проверьте свою электронную почту.'),
('conversation','text','Связь и повседневные технологии
Пример: Text me when you arrive.','
Перевод: Напиши мне, когда приедешь.'),
('conversation','internet','Связь и повседневные технологии
Пример: Is there internet here?','
Перевод: Здесь есть интернет?'),
('conversation','website','Связь и повседневные технологии
Пример: Check the website for details.','
Перевод: Посмотрите детали на сайте.'),
('conversation','password','Связь и повседневные технологии
Пример: I forgot my password.','
Перевод: Я забыл пароль.'),
('conversation','account','Связь и повседневные технологии
Пример: I cannot access my account.','
Перевод: Я не могу войти в свой аккаунт.'),
('conversation','screen','Связь и повседневные технологии
Пример: My screen is broken.','
Перевод: Мой экран сломан.'),
('conversation','computer','Связь и повседневные технологии
Пример: I use a computer at work.','
Перевод: Я использую компьютер на работе.'),
('conversation','laptop','Связь и повседневные технологии
Пример: I work on my laptop.','
Перевод: Я работаю на ноутбуке.'),
('conversation','keyboard','Связь и повседневные технологии
Пример: The keyboard is not working.','
Перевод: Клавиатура не работает.'),
('conversation','mouse','Связь и повседневные технологии
Пример: I need a new mouse.','
Перевод: Мне нужна новая мышь.'),
('conversation','button','Связь и повседневные технологии
Пример: Press this button.','
Перевод: Нажмите эту кнопку.'),
('conversation','photo','Связь и повседневные технологии
Пример: Can you take a photo?','
Перевод: Можете сделать фото?'),
('conversation','picture','Связь и повседневные технологии
Пример: Look at this picture.','
Перевод: Посмотрите на эту картинку.'),
('conversation','video','Связь и повседневные технологии
Пример: Send me the video.','
Перевод: Отправьте мне видео.'),
('conversation','camera','Связь и повседневные технологии
Пример: My camera is not working.','
Перевод: Моя камера не работает.'),
('conversation','battery','Связь и повседневные технологии
Пример: My battery is almost empty.','
Перевод: Батарея почти разряжена.'),
('conversation','cable','Связь и повседневные технологии
Пример: I need a charging cable.','
Перевод: Мне нужен зарядный кабель.'),
('conversation','plug','Связь и повседневные технологии
Пример: Plug it in here.','
Перевод: Подключите его здесь.'),
('conversation','connect','Связь и повседневные технологии
Пример: How do I connect to the internet?','
Перевод: Как подключиться к интернету?'),
('conversation','charge','Связь и повседневные технологии
Пример: Can I charge my phone here?','
Перевод: Можно здесь зарядить телефон?'),
('conversation','download','Связь и повседневные технологии
Пример: Download the form first.','
Перевод: Сначала скачайте форму.'),
('conversation','upload','Связь и повседневные технологии
Пример: Upload your photo here.','
Перевод: Загрузите свою фотографию здесь.'),
('conversation','print','Связь и повседневные технологии
Пример: Can you print this for me?','
Перевод: Можете распечатать это для меня?'),
('conversation','copy','Связь и повседневные технологии
Пример: I need a copy of my passport.','
Перевод: Мне нужна копия моего паспорта.'),
('conversation','file','Связь и повседневные технологии
Пример: I cannot open the file.','
Перевод: Я не могу открыть файл.'),
('conversation','link','Связь и повседневные технологии
Пример: Send me the link.','
Перевод: Отправьте мне ссылку.'),
('conversation','app','Связь и повседневные технологии
Пример: This app is easy to use.','
Перевод: Это приложение легко использовать.'),
('conversation','online','Связь и повседневные технологии
Пример: Can I pay online?','
Перевод: Можно оплатить онлайн?'),
('conversation','body','Тело и визит к врачу
Пример: My whole body hurts.','
Перевод: У меня болит всё тело.'),
('conversation','head','Тело и визит к врачу
Пример: I hit my head.','
Перевод: Я ударил голову.'),
('conversation','face','Тело и визит к врачу
Пример: Wash your face.','
Перевод: Помойте лицо.'),
('conversation','eye','Тело и визит к врачу
Пример: My eye hurts.','
Перевод: У меня болит глаз.'),
('conversation','ear','Тело и визит к врачу
Пример: I have pain in my ear.','
Перевод: У меня болит ухо.'),
('conversation','nose','Тело и визит к врачу
Пример: My nose is blocked.','
Перевод: У меня заложен нос.'),
('conversation','mouth','Тело и визит к врачу
Пример: Open your mouth, please.','
Перевод: Откройте рот, пожалуйста.'),
('conversation','tooth','Тело и визит к врачу
Пример: I have a broken tooth.','
Перевод: У меня сломался зуб.'),
('conversation','hair','Тело и визит к врачу
Пример: I need a haircut.','
Перевод: Мне нужна стрижка.'),
('conversation','neck','Тело и визит к врачу
Пример: My neck is stiff.','
Перевод: У меня скованная шея.'),
('conversation','shoulder','Тело и визит к врачу
Пример: My shoulder hurts.','
Перевод: У меня болит плечо.'),
('conversation','arm','Тело и визит к врачу
Пример: I broke my arm.','
Перевод: Я сломал(а) руку.'),
('conversation','hand','Тело и визит к врачу
Пример: Wash your hands.','
Перевод: Помойте руки.'),
('conversation','finger','Тело и визит к врачу
Пример: I cut my finger.','
Перевод: Я порезал(а) палец.'),
('conversation','leg','Тело и визит к врачу
Пример: My leg hurts.','
Перевод: Болит нога.'),
('conversation','knee','Тело и визит к врачу
Пример: I fell on my knee.','
Перевод: Я упал(а) на колено.'),
('conversation','foot','Тело и визит к врачу
Пример: My foot is swollen.','
Перевод: Нога отёкшая.'),
('conversation','stomach','Тело и визит к врачу
Пример: My stomach hurts.','
Перевод: Болит живот.'),
('conversation','heart','Тело и визит к врачу
Пример: My heart is beating fast.','
Перевод: Сердце бьётся быстро.'),
('conversation','skin','Тело и визит к врачу
Пример: My skin is dry.','
Перевод: Кожа сухая.'),
('conversation','pain','Тело и визит к врачу
Пример: I have pain here.','
Перевод: Здесь болит.'),
('conversation','hurt','Тело и визит к врачу
Пример: Does it hurt?','
Перевод: Болит?'),
('conversation','doctor','Тело и визит к врачу
Пример: I need to see a doctor.','
Перевод: Мне нужно к врачу.'),
('conversation','nurse','Тело и визит к врачу
Пример: The nurse will help you.','
Перевод: Медсестра вам поможет.'),
('conversation','dentist','Тело и визит к врачу
Пример: I have an appointment with the dentist.','
Перевод: У меня запись к стоматологу.'),
('conversation','appointment','Тело и визит к врачу
Пример: I would like to make an appointment.','
Перевод: Я хотел(а) бы записаться на приём.'),
('conversation','medicine','Тело и визит к врачу
Пример: Do you take any medicine?','
Перевод: Вы принимаете какие‑либо лекарства?'),
('conversation','ill','Тело и визит к врачу
Пример: I am too ill to work.','
Перевод: Я слишком болен(а), чтобы работать.'),
('conversation','health','Тело и визит к врачу
Пример: Walking is good for your health.','
Перевод: Ходьба полезна для здоровья.'),
('conversation','healthy','Тело и визит к врачу
Пример: I try to eat healthy food.','
Перевод: Я стараюсь питаться здоровой пищей.'),
('conversation','weather','Погода и природа
Пример: What is the weather like?','
Перевод: Какая погода?'),
('conversation','sun','Погода и природа
Пример: The sun is bright today.','
Перевод: Сегодня ярко светит солнце.'),
('conversation','rain','Погода и природа
Пример: It might rain later.','
Перевод: Позже может пойти дождь.'),
('conversation','snow','Погода и природа
Пример: There is snow on the road.','
Перевод: На дороге лежит снег.'),
('conversation','wind','Погода и природа
Пример: The wind is strong.','
Перевод: Ветер сильный.'),
('conversation','cloud','Погода и природа
Пример: There are dark clouds outside.','
Перевод: Снаружи тёмные облака.'),
('conversation','sky','Погода и природа
Пример: The sky is clear.','
Перевод: Небо чистое.'),
('conversation','storm','Погода и природа
Пример: A storm is coming.','
Перевод: Приближается буря.'),
('conversation','fog','Погода и природа
Пример: Drive slowly in the fog.','
Перевод: Едьте медленно в тумане.'),
('conversation','temperature','Погода и природа
Пример: What is the temperature outside?','
Перевод: Какая температура наружу?'),
('conversation','sunny','Погода и природа
Пример: It is sunny today.','
Перевод: Сегодня солнечно.'),
('conversation','rainy','Погода и природа
Пример: It was a rainy weekend.','
Перевод: Было дождливое выходные.'),
('conversation','windy','Погода и природа
Пример: It is too windy to sit outside.','
Перевод: Слишком ветрено, чтобы сидеть на улице.'),
('conversation','wet','Погода и природа
Пример: My shoes are wet.','
Перевод: Мои ботинки мокрые.'),
('conversation','dry','Погода и природа
Пример: The clothes are dry.','
Перевод: Одежда сухая.'),
('conversation','spring','Погода и природа
Пример: Flowers grow in spring.','
Перевод: Цветы растут весной.'),
('conversation','summer','Погода и природа
Пример: We swim in summer.','
Перевод: Мы плаваем летом.'),
('conversation','autumn','Погода и природа
Пример: The leaves change in autumn.','
Перевод: Листья меняются осенью.'),
('conversation','winter','Погода и природа
Пример: It is cold in winter.','
Перевод: Зимой холодно.'),
('conversation','season','Погода и природа
Пример: Summer is my favorite season.','
Перевод: Лето — моё любимое время года.'),
('conversation','sea','Погода и природа
Пример: We live near the sea.','
Перевод: Мы живём рядом с морем.'),
('conversation','river','Погода и природа
Пример: There is a bridge over the river.','
Перевод: Через реку есть мост.'),
('conversation','lake','Погода и природа
Пример: We walked around the lake.','
Перевод: Мы прогулялись вокруг озера.'),
('conversation','beach','Погода и природа
Пример: Let''s go to the beach.','
Перевод: Пойдём на пляж.'),
('conversation','mountain','Погода и природа
Пример: I can see the mountains.','
Перевод: Я вижу горы.'),
('conversation','forest','Погода и природа
Пример: We walked through the forest.','
Перевод: Мы прошли через лес.'),
('conversation','tree','Погода и природа
Пример: There is a tree outside.','
Перевод: Снаружи стоит дерево.'),
('conversation','flower','Погода и природа
Пример: These flowers are for you.','
Перевод: Эти цветы для тебя.'),
('conversation','grass','Погода и природа
Пример: Do not walk on the grass.','
Перевод: Не ходи по траве.'),
('conversation','nature','Погода и природа
Пример: I like spending time in nature.','
Перевод: Мне нравится проводить время на природе.'),
('conversation','music','Досуг и простое общение
Пример: What music do you like?','
Перевод: Какая музыка тебе нравится?'),
('conversation','song','Досуг и простое общение
Пример: I love this song.','
Перевод: Я люблю эту песню.'),
('conversation','film','Досуг и простое общение
Пример: Let''s watch a film.','
Перевод: Давай посмотрим фильм.'),
('conversation','movie','Досуг и простое общение
Пример: Have you seen this movie?','
Перевод: Ты видел этот фильм?'),
('conversation','game','Досуг и простое общение
Пример: Let''s play a game.','
Перевод: Давай сыграем в игру.'),
('conversation','sport','Досуг и простое общение
Пример: What sport do you like?','
Перевод: Какой спорт тебе нравится?'),
('conversation','football','Досуг и простое общение
Пример: We play football on Sundays.','
Перевод: Мы играем в футбол по воскресеньям.'),
('conversation','basketball','Досуг и простое общение
Пример: Do you play basketball?','
Перевод: Ты играешь в баскетбол?'),
('conversation','tennis','Досуг и простое общение
Пример: I am learning to play tennis.','
Перевод: Я учусь играть в теннис.'),
('conversation','swim','Досуг и простое общение
Пример: Can you swim?','
Перевод: Ты умеешь плавать?'),
('conversation','dance','Досуг и простое общение
Пример: I like to dance.','
Перевод: Мне нравится танцевать.'),
('conversation','sing','Досуг и простое общение
Пример: She can sing well.','
Перевод: Она хорошо поёт.'),
('conversation','play','Досуг и простое общение
Пример: The children are playing outside.','
Перевод: Дети играют на улице.'),
('conversation','visit','Досуг и простое общение
Пример: I want to visit my family.','
Перевод: Я хочу навестить свою семью.'),
('conversation','invite','Досуг и простое общение
Пример: Can I invite a friend?','
Перевод: Можно пригласить друга?'),
('conversation','party','Досуг и простое общение
Пример: Are you coming to the party?','
Перевод: Ты придёшь на вечеринку?'),
('conversation','birthday','Досуг и простое общение
Пример: Happy birthday!','
Перевод: С днём рождения!'),
('conversation','gift','Досуг и простое общение
Пример: This is a gift for you.','
Перевод: Это подарок для тебя.'),
('conversation','present','Досуг и простое общение
Пример: I bought a present for my sister.','
Перевод: Я купил подарок для сестры.'),
('conversation','holiday','Досуг и простое общение
Пример: I am on holiday.','
Перевод: Я в отпуске.'),
('conversation','vacation','Досуг и простое общение
Пример: When is your vacation?','
Перевод: Когда у тебя отпуск?'),
('conversation','fun','Досуг и простое общение
Пример: We had fun.','
Перевод: Мы хорошо провели время.'),
('conversation','funny','Досуг и простое общение
Пример: That is a funny story.','
Перевод: Это смешная история.'),
('conversation','interesting','Досуг и простое общение
Пример: That sounds interesting.','
Перевод: Звучит интересно.'),
('conversation','boring','Досуг и простое общение
Пример: The film was boring.','
Перевод: Фильм был скучным.'),
('conversation','favorite','Досуг и простое общение
Пример: What is your favorite food?','
Перевод: Какая твоя любимая еда?'),
('conversation','hobby','Досуг и простое общение
Пример: Cooking is my hobby.','
Перевод: Готовка — моё хобби.'),
('conversation','relax','Досуг и простое общение
Пример: I want to relax at home.','
Перевод: Я хочу отдохнуть дома.'),
('conversation','journey','Движение и дорога
Пример: How was your journey?','
Перевод: Как прошла твоя поездка?'),
('conversation','trip','Движение и дорога
Пример: Have a good trip.','
Перевод: Счастливого пути.'),
('conversation','flight','Движение и дорога
Пример: My flight is delayed.','
Перевод: Мой рейс задерживается.'),
('conversation','luggage','Движение и дорога
Пример: Where can I leave my luggage?','
Перевод: Где я могу оставить свой багаж?'),
('conversation','seat','Движение и дорога
Пример: Is this seat taken?','
Перевод: Это место занято?'),
('conversation','passenger','Движение и дорога
Пример: All passengers must show a ticket.','
Перевод: Все пассажиры должны предъявить билет.'),
('conversation','driver','Движение и дорога
Пример: Ask the driver.','
Перевод: Спросите у водителя.'),
('conversation','traffic','Движение и дорога
Пример: There is a lot of traffic.','
Перевод: На дорогах сильный трафик.'),
('conversation','cross','Движение и дорога
Пример: Cross at the traffic lights.','
Перевод: Перейдите на светофоре.'),
('conversation','bridge','Движение и дорога
Пример: Go across the bridge.','
Перевод: Перейдите через мост.'),
('conversation','corner','Движение и дорога
Пример: The shop is on the corner.','
Перевод: Магазин находится на углу.'),
('conversation','side','Движение и дорога
Пример: Walk on this side of the road.','
Перевод: Идите по этой стороне дороги.'),
('conversation','end','Движение и дорога
Пример: The bank is at the end of the street.','
Перевод: Банк находится в конце улицы.'),
('conversation','front','Движение и дорога
Пример: Wait in front of the building.','
Перевод: Ждите перед зданием.'),
('conversation','center','Движение и дорога
Пример: How do I get to the city center?','
Перевод: Как добраться до центра города?'),
('conversation','north','Движение и дорога
Пример: Drive north for ten minutes.','
Перевод: Едьте на север десять минут.'),
('conversation','south','Движение и дорога
Пример: We live south of the city.','
Перевод: Мы живём к югу от города.'),
('conversation','east','Движение и дорога
Пример: The airport is east of here.','
Перевод: Аэропорт находится к востоку отсюда.'),
('conversation','west','Движение и дорога
Пример: We are heading west.','
Перевод: Мы едем на запад.'),
('conversation','line','Движение и дорога
Пример: Which line goes to the airport?','
Перевод: Какой маршрут идёт до аэропорта?'),
('conversation','queue','Движение и дорога
Пример: Is this the queue for tickets?','
Перевод: Это очередь за билетами?'),
('conversation','platform','Движение и дорога
Пример: Which platform does it leave from?','
Перевод: С какого платформы отправляется?'),
('conversation','delay','Движение и дорога
Пример: There is a short delay.','
Перевод: Есть небольшая задержка.'),
('conversation','cancel','Движение и дорога
Пример: I need to cancel my booking.','
Перевод: Мне нужно отменить бронирование.'),
('conversation','miss','Движение и дорога
Пример: I missed the train.','
Перевод: Я опоздал на поезд.'),
('conversation','catch','Движение и дорога
Пример: I need to catch the bus.','
Перевод: Мне нужно успеть на автобус.'),
('conversation','reach','Движение и дорога
Пример: We should reach the hotel by six.','
Перевод: Мы должны быть в отеле к шести.'),
('conversation','direction','Движение и дорога
Пример: Are we going in the right direction?','
Перевод: Мы едем в правильном направлении?'),
('conversation','distance','Движение и дорога
Пример: What is the distance to the airport?','
Перевод: Каково расстояние до аэропорта?'),
('conversation','welcome','Вежливость и разговор
Пример: Welcome to our home.','
Перевод: Добро пожаловать в наш дом.'),
('conversation','goodbye','Вежливость и разговор
Пример: Goodbye, see you soon.','
Перевод: До свидания, скоро увидимся.'),
('conversation','thanks','Вежливость и разговор
Пример: Thanks for your help.','
Перевод: Спасибо за вашу помощь.'),
('conversation','congratulations','Вежливость и разговор
Пример: Congratulations on your new job!','
Перевод: Поздравляем с новой работой!'),
('conversation','wish','Вежливость и разговор
Пример: I wish you all the best.','
Перевод: Желаю вам всего наилучшего.'),
('conversation','luck','Вежливость и разговор
Пример: Good luck with your interview.','
Перевод: Удачи на собеседовании.'),
('conversation','cheers','Вежливость и разговор
Пример: Cheers, see you later.','
Перевод: Пока, увидимся позже.'),
('conversation','smile','Вежливость и разговор
Пример: She smiled at me.','
Перевод: Она улыбнулась мне.'),
('conversation','laugh','Вежливость и разговор
Пример: That made me laugh.','
Перевод: Это заставило меня смеяться.'),
('conversation','joke','Вежливость и разговор
Пример: It was just a joke.','
Перевод: Это была просто шутка.'),
('conversation','chat','Вежливость и разговор
Пример: Do you have time for a chat?','
Перевод: У вас есть время для разговора?'),
('conversation','talk','Вежливость и разговор
Пример: Can we talk for a minute?','
Перевод: Можем поговорить минутку?'),
('conversation','conversation','Вежливость и разговор
Пример: We had a good conversation.','
Перевод: У нас был хороший разговор.'),
('conversation','discuss','Вежливость и разговор
Пример: Let''s discuss this tomorrow.','
Перевод: Обсудим это завтра.'),
('conversation','share','Вежливость и разговор
Пример: Can we share a taxi?','
Перевод: Можно поделить такси?'),
('conversation','offer','Вежливость и разговор
Пример: Can I offer you some tea?','
Перевод: Можно предложить вам чай?'),
('conversation','accept','Вежливость и разговор
Пример: I accept your offer.','
Перевод: Я принимаю ваше предложение.'),
('conversation','refuse','Вежливость и разговор
Пример: You can refuse if you want.','
Перевод: Вы можете отказаться, если хотите.'),
('conversation','apologize','Вежливость и разговор
Пример: I want to apologize for being late.','
Перевод: Я хочу извиниться за опоздание.'),
('conversation','promise','Вежливость и разговор
Пример: I promise to call you.','
Перевод: Обещаю вам позвонить.'),
('conversation','respect','Вежливость и разговор
Пример: I respect your decision.','
Перевод: Я уважаю ваше решение.'),
('conversation','trust','Вежливость и разговор
Пример: I trust you.','
Перевод: Я вам доверяю.'),
('conversation','support','Вежливость и разговор
Пример: Thank you for your support.','
Перевод: Спасибо за вашу поддержку.'),
('conversation','care','Вежливость и разговор
Пример: I care about my family.','
Перевод: Я забочусь о своей семье.'),
('conversation','mind','Вежливость и разговор
Пример: Do you mind if I sit here?','
Перевод: Вы не возражаете, если я сяду здесь?'),
('conversation','allow','Вежливость и разговор
Пример: Do they allow dogs here?','
Перевод: Разрешают ли здесь собак?'),
('conversation','permission','Вежливость и разговор
Пример: Do I need permission?','
Перевод: Нужна ли мне разрешение?'),
('conversation','polite','Вежливость и разговор
Пример: He was very polite.','
Перевод: Он был очень вежливым.'),
('conversation','rude','Вежливость и разговор
Пример: That sounded rude.','
Перевод: Это звучало грубо.'),
('conversation','honest','Вежливость и разговор
Пример: Please be honest with me.','
Перевод: Пожалуйста, будьте со мной честны.'),
('conversation','nice','Качества и сравнения
Пример: It is nice to see you.','
Перевод: Приятно вас видеть.'),
('conversation','beautiful','Качества и сравнения
Пример: What a beautiful place.','
Перевод: Какое красивое место!'),
('conversation','ugly','Качества и сравнения
Пример: I think that building is ugly.','
Перевод: Я считаю это здание уродливым.'),
('conversation','pretty','Качества и сравнения
Пример: That is a pretty dress.','
Перевод: Это красивое платье.'),
('conversation','heavy','Качества и сравнения
Пример: This bag is heavy.','
Перевод: Эта сумка тяжёлая.'),
('conversation','soft','Качества и сравнения
Пример: The bed is too soft.','
Перевод: Кровать слишком мягкая.'),
('conversation','thick','Качества и сравнения
Пример: Wear thick socks.','
Перевод: Носите плотные носки.'),
('conversation','thin','Качества и сравнения
Пример: The walls are thin.','
Перевод: Стены тонкие.'),
('conversation','wide','Качества и сравнения
Пример: The road is wide.','
Перевод: Дорога широкая.'),
('conversation','narrow','Качества и сравнения
Пример: This street is narrow.','
Перевод: Эта улица узкая.'),
('conversation','deep','Качества и сравнения
Пример: The water is deep here.','
Перевод: Здесь вода глубокая.'),
('conversation','shallow','Качества и сравнения
Пример: Stay in the shallow water.','
Перевод: Оставайтесь в мелкой воде.'),
('conversation','tall','Качества и сравнения
Пример: My brother is tall.','
Перевод: Мой брат высокий.'),
('conversation','fast','Качества и сравнения
Пример: You speak too fast.','
Перевод: Вы говорите слишком быстро.'),
('conversation','slow','Качества и сравнения
Пример: The internet is slow today.','
Перевод: Сегодня интернет медленный.'),
('conversation','quick','Качества и сравнения
Пример: Can I ask a quick question?','
Перевод: Можно задать быстрый вопрос?'),
('conversation','weak','Качества и сравнения
Пример: I feel weak today.','
Перевод: Я чувствую себя слабым сегодня.'),
('conversation','rich','Качества и сравнения
Пример: You do not need to be rich.','
Перевод: Вам не нужно быть богатым.'),
('conversation','poor','Качества и сравнения
Пример: The connection is poor.','
Перевод: Связь плохая.'),
('conversation','smart','Качества и сравнения
Пример: That is a smart idea.','
Перевод: Это умная идея.'),
('conversation','clever','Качества и сравнения
Пример: That was a clever solution.','
Перевод: Это было хитрое решение.'),
('conversation','serious','Качества и сравнения
Пример: Is it a serious problem?','
Перевод: Это серьёзная проблема?'),
('conversation','normal','Качества и сравнения
Пример: Is this normal?','
Перевод: Это нормально?'),
('conversation','strange','Качества и сравнения
Пример: That sounds strange.','
Перевод: Это звучит странно.'),
('conversation','special','Качества и сравнения
Пример: Today is a special day.','
Перевод: Сегодня особенный день.'),
('conversation','common','Качества и сравнения
Пример: This is a common problem.','
Перевод: Это распространённая проблема.'),
('conversation','real','Качества и сравнения
Пример: Is that a real photo?','
Перевод: Это реальное фото?'),
('conversation','local','Качества и сравнения
Пример: Ask a local person.','
Перевод: Спросите у местного жителя.'),
('conversation','public','Качества и сравнения
Пример: I use public transport.','
Перевод: Я пользуюсь общественным транспортом.'),
('conversation','private','Качества и сравнения
Пример: This is a private conversation.','
Перевод: Это частный разговор.'),
('conversation','life','Рассказываем о себе и жизни
Пример: Life is different here.','
Перевод: Здесь жизнь отличается.'),
('conversation','age','Рассказываем о себе и жизни
Пример: What age is your son?','
Перевод: Сколько лет вашему сыну?'),
('conversation','born','Рассказываем о себе и жизни
Пример: I was born in a small town.','
Перевод: Я родился в небольшом городе.'),
('conversation','grow','Рассказываем о себе и жизни
Пример: I grew up in the city.','
Перевод: Я вырос в городе.'),
('conversation','adult','Рассказываем о себе и жизни
Пример: Two adults and one child, please.','
Перевод: Два взрослых и один ребёнок, пожалуйста.'),
('conversation','teenager','Рассказываем о себе и жизни
Пример: My son is a teenager.','
Перевод: Моему сыну подростковый возраст.'),
('conversation','couple','Рассказываем о себе и жизни
Пример: We stayed for a couple of days.','
Перевод: Мы пробыли несколько дней.'),
('conversation','relative','Рассказываем о себе и жизни
Пример: I have relatives here.','
Перевод: У меня здесь родственники.'),
('conversation','grandmother','Рассказываем о себе и жизни
Пример: My grandmother lives with us.','
Перевод: Моя бабушка живёт с нами.'),
('conversation','grandfather','Рассказываем о себе и жизни
Пример: My grandfather taught me to swim.','
Перевод: Дедушка научил меня плавать.'),
('conversation','grandparent','Рассказываем о себе и жизни
Пример: My grandparents live nearby.','
Перевод: Мои бабушка и дедушка живут рядом.'),
('conversation','grandchild','Рассказываем о себе и жизни
Пример: She has three grandchildren.','
Перевод: У неё трое внуков.'),
('conversation','uncle','Рассказываем о себе и жизни
Пример: My uncle is a driver.','
Перевод: Мой дядя — водитель.'),
('conversation','aunt','Рассказываем о себе и жизни
Пример: I am visiting my aunt.','
Перевод: Я навещаю свою тётю.'),
('conversation','cousin','Рассказываем о себе и жизни
Пример: My cousin lives abroad.','
Перевод: Мой кузен живёт за границей.'),
('conversation','niece','Рассказываем о себе и жизни
Пример: My niece is starting school.','
Перевод: Моя племянница идёт в школу.'),
('conversation','nephew','Рассказываем о себе и жизни
Пример: My nephew is five.','
Перевод: Моему племяннику пять лет.'),
('conversation','boyfriend','Рассказываем о себе и жизни
Пример: This is my boyfriend.','
Перевод: Это мой парень.'),
('conversation','girlfriend','Рассказываем о себе и жизни
Пример: My girlfriend works here.','
Перевод: Моя девушка работает здесь.'),
('conversation','wedding','Рассказываем о себе и жизни
Пример: We are going to a wedding.','
Перевод: Мы идём на свадьбу.'),
('conversation','marry','Рассказываем о себе и жизни
Пример: They want to marry next year.','
Перевод: Они хотят пожениться в следующем году.'),
('conversation','divorce','Рассказываем о себе и жизни
Пример: They decided to get a divorce.','
Перевод: Они решили развестись.'),
('conversation','relationship','Рассказываем о себе и жизни
Пример: We have a good relationship.','
Перевод: У нас хорошие отношения.'),
('conversation','childhood','Рассказываем о себе и жизни
Пример: I had a happy childhood.','
Перевод: У меня было счастливое детство.'),
('conversation','background','Рассказываем о себе и жизни
Пример: Tell me about your background.','
Перевод: Расскажите о своём прошлом.'),
('conversation','country','Рассказываем о себе и жизни
Пример: Which country are you from?','
Перевод: Из какой вы страны?'),
('conversation','nationality','Рассказываем о себе и жизни
Пример: What is your nationality?','
Перевод: Какова ваша национальность?'),
('conversation','foreign','Рассказываем о себе и жизни
Пример: I am learning a foreign language.','
Перевод: Я изучаю иностранный язык.'),
('conversation','abroad','Рассказываем о себе и жизни
Пример: I want to work abroad.','
Перевод: Я хочу работать за границей.'),
('conversation','hometown','Рассказываем о себе и жизни
Пример: I miss my hometown.','
Перевод: Я скучаю по своему родному городу.'),
('conversation','begin','Действия, которые нужны в рассказе
Пример: Let''s begin with a simple question.','
Перевод: Начнём с простого вопроса.'),
('conversation','continue','Действия, которые нужны в рассказе
Пример: Please continue.','
Перевод: Пожалуйста, продолжайте.'),
('conversation','become','Действия, которые нужны в рассказе
Пример: I want to become a doctor.','
Перевод: Я хочу стать врачом.'),
('conversation','lose','Действия, которые нужны в рассказе
Пример: Do not lose your ticket.','
Перевод: Не потеряйте свой билет.'),
('conversation','win','Действия, которые нужны в рассказе
Пример: Our team won the game.','
Перевод: Наша команда выиграла игру.'),
('conversation','fail','Действия, которые нужны в рассказе
Пример: I failed my driving test.','
Перевод: Я провалил экзамен по вождению.'),
('conversation','pass','Действия, которые нужны в рассказе
Пример: I passed the test.','
Перевод: Я сдал экзамен.'),
('conversation','fall','Действия, которые нужны в рассказе
Пример: Be careful not to fall.','
Перевод: Будьте осторожны, чтобы не упасть.'),
('conversation','rise','Действия, которые нужны в рассказе
Пример: Prices are rising.','
Перевод: Цены растут.'),
('conversation','pull','Действия, которые нужны в рассказе
Пример: Pull the door toward you.','
Перевод: Тяните дверь к себе.'),
('conversation','push','Действия, которые нужны в рассказе
Пример: Push this door to open it.','
Перевод: Толкните эту дверь, чтобы открыть её.'),
('conversation','throw','Действия, которые нужны в рассказе
Пример: Throw the ball to me.','
Перевод: Бросьте мяч мне.'),
('conversation','hit','Действия, которые нужны в рассказе
Пример: I hit my knee on the table.','
Перевод: Я ударил колено о стол.'),
('conversation','build','Действия, которые нужны в рассказе
Пример: They are building a new school.','
Перевод: Строят новую школу.'),
('conversation','repair','Действия, которые нужны в рассказе
Пример: Can you repair my phone?','
Перевод: Вы можете отремонтировать мой телефон?'),
('conversation','fix','Действия, которые нужны в рассказе
Пример: Can you fix this?','
Перевод: Вы можете это исправить?'),
('conversation','cover','Действия, которые нужны в рассказе
Пример: Cover the food, please.','
Перевод: Покройте еду, пожалуйста.'),
('conversation','fill','Действия, которые нужны в рассказе
Пример: Fill the bottle with water.','
Перевод: Наполните бутылку водой.'),
('conversation','collect','Действия, которые нужны в рассказе
Пример: I need to collect my parcel.','
Перевод: Мне нужно забрать свою посылку.'),
('conversation','join','Действия, которые нужны в рассказе
Пример: Can I join you?','
Перевод: Можно присоединиться к вам?'),
('conversation','include','Действия, которые нужны в рассказе
Пример: Does the price include breakfast?','
Перевод: Включён ли завтрак в цену?'),
('conversation','contain','Действия, которые нужны в рассказе
Пример: Does this contain milk?','
Перевод: Содержит ли это молоко?'),
('conversation','remove','Действия, которые нужны в рассказе
Пример: Please remove your shoes.','
Перевод: Пожалуйста, снимите обувь.'),
('conversation','replace','Действия, которые нужны в рассказе
Пример: I need to replace the battery.','
Перевод: Мне нужно заменить батарею.'),
('conversation','prepare','Действия, которые нужны в рассказе
Пример: I need to prepare for the meeting.','
Перевод: Мне нужно подготовиться к встрече.'),
('conversation','improve','Действия, которые нужны в рассказе
Пример: I want to improve my English.','
Перевод: Я хочу улучшить свой английский.'),
('conversation','manage','Действия, которые нужны в рассказе
Пример: I can manage on my own.','
Перевод: Я могу справиться сам.'),
('conversation','although','Связываем мысли
Пример: I went out although it was raining.','
Перевод: Я вышел, хотя шёл дождь.'),
('conversation','though','Связываем мысли
Пример: It is expensive. I like it, though.','
Перевод: Это дорого. Но мне это нравится.'),
('conversation','while','Связываем мысли
Пример: Wait here while I get my bag.','
Перевод: Подождите здесь, пока я возьму свою сумку.'),
('conversation','unless','Связываем мысли
Пример: I will come unless I have to work.','
Перевод: Я приду, если только не придётся работать.'),
('conversation','whether','Связываем мысли
Пример: I do not know whether I can come.','
Перевод: Я не знаю, смогу ли прийти.'),
('conversation','than','Связываем мысли
Пример: This one is cheaper than that one.','
Перевод: Этот дешевле того.'),
('conversation','as','Связываем мысли
Пример: I work as a driver.','
Перевод: Я работаю водителем.'),
('conversation','except','Связываем мысли
Пример: Everyone came except Alex.','
Перевод: Все пришли, кроме Алекса.'),
('conversation','instead','Связываем мысли
Пример: Let''s walk instead.','
Перевод: Давайте лучше пройдёмся пешком.'),
('conversation','however','Связываем мысли
Пример: It is small. However, it is comfortable.','
Перевод: Он маленький, однако удобный.'),
('conversation','especially','Связываем мысли
Пример: I like fruit, especially apples.','
Перевод: Я люблю фрукты, особенно яблоки.'),
('conversation','actually','Связываем мысли
Пример: Actually, I live nearby.','
Перевод: На самом деле я живу рядом.'),
('conversation','basically','Связываем мысли
Пример: Basically, we need more time.','
Перевод: В основном нам нужно больше времени.'),
('conversation','exactly','Связываем мысли
Пример: That is exactly what I mean.','
Перевод: Это именно то, что я имею в виду.'),
('conversation','simply','Связываем мысли
Пример: Simply press this button.','
Перевод: Просто нажмите эту кнопку.'),
('conversation','finally','Связываем мысли
Пример: We finally got home.','
Перевод: Мы наконец‑то добрались домой.'),
('conversation','fortunately','Связываем мысли
Пример: Fortunately, nobody was hurt.','
Перевод: К счастью, никто не пострадал.'),
('conversation','unfortunately','Связываем мысли
Пример: Unfortunately, I cannot come.','
Перевод: К сожалению, я не смогу прийти.'),
('conversation','anyway','Связываем мысли
Пример: Thanks anyway.','
Перевод: Всё равно спасибо.'),
('conversation','otherwise','Связываем мысли
Пример: Leave now, otherwise you will be late.','
Перевод: Уходи сейчас, иначе опоздаешь.'),
('conversation','else','Связываем мысли
Пример: Anything else?','
Перевод: Что-нибудь ещё?'),
('conversation','either','Связываем мысли
Пример: Either day is fine.','
Перевод: Любой из дней подходит.'),
('conversation','neither','Связываем мысли
Пример: Neither option works for me.','
Перевод: Ни один вариант мне не подходит.'),
('conversation','everything','Связываем мысли
Пример: Is everything OK?','
Перевод: Всё в порядке?'),
('conversation','something','Связываем мысли
Пример: I need something to eat.','
Перевод: Мне нужно что‑нибудь поесть.'),
('conversation','anything','Связываем мысли
Пример: Do you need anything?','
Перевод: Тебе что‑нибудь нужно?'),
('conversation','nothing','Связываем мысли
Пример: Nothing is wrong.','
Перевод: Ничего не случилось.'),
('conversation','whatever','Связываем мысли
Пример: Choose whatever you like.','
Перевод: Выбирай, что тебе нравится.'),
('conversation','someone','Люди, места и частота
Пример: Someone is at the door.','
Перевод: Кто‑то у двери.'),
('conversation','anyone','Люди, места и частота
Пример: Can anyone help me?','
Перевод: Может кто‑нибудь помочь мне?'),
('conversation','everyone','Люди, места и частота
Пример: Everyone is ready.','
Перевод: Все готовы.'),
('conversation','nobody','Люди, места и частота
Пример: Nobody answered the phone.','
Перевод: Никто не ответил на телефон.'),
('conversation','somebody','Люди, места и частота
Пример: Somebody left a bag here.','
Перевод: Кто‑то оставил здесь сумку.'),
('conversation','anybody','Люди, места и частота
Пример: Is anybody there?','
Перевод: Есть кто‑нибудь?'),
('conversation','everybody','Люди, места и частота
Пример: Everybody needs a break.','
Перевод: Каждому нужен перерыв.'),
('conversation','anywhere','Люди, места и частота
Пример: Can I park anywhere here?','
Перевод: Можно здесь где‑угодно парковаться?'),
('conversation','somewhere','Люди, места и частота
Пример: Let''s go somewhere quiet.','
Перевод: Пойдём в тихое место.'),
('conversation','everywhere','Люди, места и частота
Пример: I looked everywhere.','
Перевод: Я искал везде.'),
('conversation','nowhere','Люди, места и частота
Пример: There is nowhere to park.','
Перевод: Нет места для парковки.'),
('conversation','whenever','Люди, места и частота
Пример: Call whenever you need help.','
Перевод: Звони, когда понадобится помощь.'),
('conversation','wherever','Люди, места и частота
Пример: Sit wherever you like.','
Перевод: Сиди, где тебе удобно.'),
('conversation','once','Люди, места и частота
Пример: I go there once a week.','
Перевод: Я хожу туда раз в неделю.'),
('conversation','twice','Люди, места и частота
Пример: Take a break twice a day.','
Перевод: Делай перерыв два раза в день.'),
('conversation','daily','Люди, места и частота
Пример: I practice daily.','
Перевод: Я практикую каждый день.'),
('conversation','weekly','Люди, места и частота
Пример: We have a weekly meeting.','
Перевод: У нас еженедельное собрание.'),
('conversation','monthly','Люди, места и частота
Пример: What is the monthly payment?','
Перевод: Какой размер ежемесячного платежа?'),
('conversation','regularly','Люди, места и частота
Пример: I exercise regularly.','
Перевод: Я регулярно занимаюсь спортом.'),
('conversation','rarely','Люди, места и частота
Пример: I rarely eat out.','
Перевод: Я редко ем вне дома.'),
('conversation','recently','Люди, места и частота
Пример: I recently changed jobs.','
Перевод: Я недавно сменил работу.'),
('conversation','lately','Люди, места и частота
Пример: I have been busy lately.','
Перевод: В последнее время я был занят.'),
('conversation','previously','Люди, места и частота
Пример: I previously worked in a shop.','
Перевод: Раньше я работал в магазине.'),
('conversation','currently','Люди, места и частота
Пример: I currently live here.','
Перевод: Сейчас я живу здесь.'),
('conversation','immediately','Люди, места и частота
Пример: Please call me immediately.','
Перевод: Пожалуйста, позвоните мне сразу же.'),
('conversation','eventually','Люди, места и частота
Пример: We eventually found the hotel.','
Перевод: В конце концов мы нашли отель.'),
('conversation','suddenly','Люди, места и частота
Пример: The lights suddenly went out.','
Перевод: Свет внезапно погас.'),
('conversation','definitely','Люди, места и частота
Пример: I will definitely come.','
Перевод: Я точно приду.'),
('conversation','feeling','Чувства и характер глубже
Пример: Tell me how you are feeling.','
Перевод: Скажи, как ты себя чувствуешь.'),
('conversation','emotion','Чувства и характер глубже
Пример: It is hard to describe this emotion.','
Перевод: Трудно описать это чувство.'),
('conversation','mood','Чувства и характер глубже
Пример: I am in a good mood.','
Перевод: Я в хорошем настроении.'),
('conversation','fear','Чувства и характер глубже
Пример: I have a fear of flying.','
Перевод: Я боюсь летать.'),
('conversation','worry','Чувства и характер глубже
Пример: Do not worry about it.','
Перевод: Не переживайте об этом.'),
('conversation','stress','Чувства и характер глубже
Пример: Work causes me a lot of stress.','
Перевод: Работа причиняет мне много стресса.'),
('conversation','pressure','Чувства и характер глубже
Пример: I work well under pressure.','
Перевод: Я хорошо работаю под давлением.'),
('conversation','confidence','Чувства и характер глубже
Пример: Practice gives me confidence.','
Перевод: Практика даёт мне уверенность.'),
('conversation','confident','Чувства и характер глубже
Пример: I feel more confident now.','
Перевод: Сейчас я чувствую себя более уверенно.'),
('conversation','shy','Чувства и характер глубже
Пример: I am shy around new people.','
Перевод: Я стеснителен с новыми людьми.'),
('conversation','proud','Чувства и характер глубже
Пример: I am proud of my progress.','
Перевод: Я горжусь своим прогрессом.'),
('conversation','ashamed','Чувства и характер глубже
Пример: I felt ashamed of my mistake.','
Перевод: Мне было стыдно из‑за моей ошибки.'),
('conversation','embarrassed','Чувства и характер глубже
Пример: I was embarrassed to ask.','
Перевод: Мне было неловко спрашивать.'),
('conversation','lonely','Чувства и характер глубже
Пример: I feel lonely sometimes.','
Перевод: Иногда я чувствую себя одиноко.'),
('conversation','upset','Чувства и характер глубже
Пример: I am upset about what happened.','
Перевод: Я расстроен тем, что случилось.'),
('conversation','disappointed','Чувства и характер глубже
Пример: I was disappointed with the service.','
Перевод: Я был разочарован обслуживанием.'),
('conversation','pleased','Чувства и характер глубже
Пример: I am pleased with the result.','
Перевод: Я доволен результатом.'),
('conversation','grateful','Чувства и характер глубже
Пример: I am grateful for your help.','
Перевод: Я благодарен за вашу помощь.'),
('conversation','relieved','Чувства и характер глубже
Пример: I am relieved you are safe.','
Перевод: Я рад, что вы в безопасности.'),
('conversation','confused','Чувства и характер глубже
Пример: I am confused. Can you explain?','
Перевод: Я в замешательстве. Можете объяснить?'),
('conversation','curious','Чувства и характер глубже
Пример: I am curious about your work.','
Перевод: Мне любопытно, чем вы занимаетесь.'),
('conversation','patient','Чувства и характер глубже
Пример: Please be patient with me.','
Перевод: Пожалуйста, будьте терпеливы ко мне.'),
('conversation','lazy','Чувства и характер глубже
Пример: I feel lazy today.','
Перевод: Сегодня я чувствую себя ленивым.'),
('conversation','active','Чувства и характер глубже
Пример: I try to stay active.','
Перевод: Я стараюсь оставаться активным.'),
('conversation','positive','Чувства и характер глубже
Пример: Try to stay positive.','
Перевод: Старайтесь мыслить позитивно.'),
('conversation','negative','Чувства и характер глубже
Пример: I got a negative answer.','
Перевод: Я получил отрицательный ответ.'),
('conversation','generous','Чувства и характер глубже
Пример: That is very generous of you.','
Перевод: Это очень щедро с вашей стороны.'),
('conversation','selfish','Чувства и характер глубже
Пример: I do not want to seem selfish.','
Перевод: Я не хочу казаться эгоистом.'),
('conversation','fair','Чувства и характер глубже
Пример: That seems fair.','
Перевод: Это кажется справедливым.'),
('conversation','unfair','Чувства и характер глубже
Пример: That is unfair to the others.','
Перевод: Это несправедливо по отношению к другим.'),
('conversation','exercise','Спорт, тренировки и привычки
Пример: I exercise three times a week.','
Перевод: Я занимаюсь спортом три раза в неделю.'),
('conversation','training','Спорт, тренировки и привычки
Пример: Training starts at six.','
Перевод: Тренировка начинается в шесть.'),
('conversation','gym','Спорт, тренировки и привычки
Пример: I go to the gym after work.','
Перевод: Я хожу в спортзал после работы.'),
('conversation','coach','Спорт, тренировки и привычки
Пример: My coach helps me improve.','
Перевод: Мой тренер помогает мне улучшаться.'),
('conversation','player','Спорт, тренировки и привычки
Пример: He is a good player.','
Перевод: Он хороший игрок.'),
('conversation','match','Спорт, тренировки и привычки
Пример: Did you watch the match?','
Перевод: Вы смотрели матч?'),
('conversation','competition','Спорт, тренировки и привычки
Пример: I am preparing for a competition.','
Перевод: Я готовлюсь к соревнованиям.'),
('conversation','race','Спорт, тренировки и привычки
Пример: I finished the race.','
Перевод: Я закончил гонку.'),
('conversation','fight','Спорт, тренировки и привычки
Пример: I watched the fight last night.','
Перевод: Я смотрел бой вчера вечером.'),
('conversation','boxing','Спорт, тренировки и привычки
Пример: I enjoy boxing.','
Перевод: Мне нравится бокс.'),
('conversation','wrestling','Спорт, тренировки и привычки
Пример: Wrestling requires strength.','
Перевод: Для борьбы нужна сила.'),
('conversation','strength','Спорт, тренировки и привычки
Пример: I want to build strength.','
Перевод: Я хочу набрать силу.'),
('conversation','energy','Спорт, тренировки и привычки
Пример: I have more energy now.','
Перевод: У меня теперь больше энергии.'),
('conversation','speed','Спорт, тренировки и привычки
Пример: Reduce your speed.','
Перевод: Снизьте скорость.'),
('conversation','balance','Спорт, тренировки и привычки
Пример: I lost my balance.','
Перевод: Я потерял равновесие.'),
('conversation','weight','Спорт, тренировки и привычки
Пример: I want to maintain my weight.','
Перевод: Я хочу поддерживать вес.'),
('conversation','goal','Спорт, тренировки и привычки
Пример: My goal is to speak clearly.','
Перевод: Моя цель — говорить чётко.'),
('conversation','effort','Спорт, тренировки и привычки
Пример: It takes time and effort.','
Перевод: Это требует времени и усилий.'),
('conversation','habit','Спорт, тренировки и привычки
Пример: Walking is a healthy habit.','
Перевод: Ходьба — полезная привычка.'),
('conversation','routine','Спорт, тренировки и привычки
Пример: This is my morning routine.','
Перевод: Это мой утренний распорядок.'),
('conversation','rest','Спорт, тренировки и привычки
Пример: You need some rest.','
Перевод: Вам нужен отдых.'),
('conversation','stretch','Спорт, тренировки и привычки
Пример: I stretch after training.','
Перевод: Я растягиваюсь после тренировки.'),
('conversation','breathe','Спорт, тренировки и привычки
Пример: Breathe slowly.','
Перевод: Дышите медленно.'),
('conversation','jump','Спорт, тренировки и привычки
Пример: The children love to jump.','
Перевод: Детям нравится прыгать.'),
('conversation','climb','Спорт, тренировки и привычки
Пример: Can you climb the stairs?','
Перевод: Вы можете подняться по лестнице?'),
('conversation','lift','Спорт, тренировки и привычки
Пример: Can you help me lift this?','
Перевод: Поможете мне поднять это?'),
('conversation','kick','Спорт, тренировки и привычки
Пример: Kick the ball to me.','
Перевод: Пинайте мяч ко мне.'),
('conversation','ball','Спорт, тренировки и привычки
Пример: Throw me the ball.','
Перевод: Бросьте мне мяч.'),
('conversation','progress','Спорт, тренировки и привычки
Пример: I am making progress.','
Перевод: Я делаю успехи.'),
('conversation','cough','Здоровье: объяснить проблему
Пример: I have a cough.','
Перевод: У меня кашель.'),
('conversation','fever','Здоровье: объяснить проблему
Пример: I have a fever.','
Перевод: У меня температура.'),
('conversation','headache','Здоровье: объяснить проблему
Пример: I have a headache.','
Перевод: У меня головная боль.'),
('conversation','toothache','Здоровье: объяснить проблему
Пример: I have toothache.','
Перевод: У меня зубная боль.'),
('conversation','sore','Здоровье: объяснить проблему
Пример: I have a sore throat.','
Перевод: У меня боль в горле.'),
('conversation','throat','Здоровье: объяснить проблему
Пример: My throat hurts.','
Перевод: Меня болит горло.'),
('conversation','flu','Здоровье: объяснить проблему
Пример: I think I have the flu.','
Перевод: Я думаю, у меня грипп.'),
('conversation','allergy','Здоровье: объяснить проблему
Пример: I have a food allergy.','
Перевод: У меня пищевая аллергия.'),
('conversation','allergic','Здоровье: объяснить проблему
Пример: I am allergic to nuts.','
Перевод: У меня аллергия на орехи.'),
('conversation','asthma','Здоровье: объяснить проблему
Пример: I have asthma.','
Перевод: У меня астма.'),
('conversation','infection','Здоровье: объяснить проблему
Пример: The doctor checked for an infection.','
Перевод: Врач проверил наличие инфекции.'),
('conversation','injury','Здоровье: объяснить проблему
Пример: He has a knee injury.','
Перевод: У него травма колена.'),
('conversation','accident','Здоровье: объяснить проблему
Пример: There was an accident.','
Перевод: Произошёл несчастный случай.'),
('conversation','wound','Здоровье: объяснить проблему
Пример: Keep the wound clean.','
Перевод: Держите рану в чистоте.'),
('conversation','blood','Здоровье: объяснить проблему
Пример: There is blood on my shirt.','
Перевод: На моей рубашке кровь.'),
('conversation','bleed','Здоровье: объяснить проблему
Пример: My nose is bleeding.','
Перевод: У меня кровотечение из носа.'),
('conversation','bruise','Здоровье: объяснить проблему
Пример: I have a bruise on my leg.','
Перевод: У меня синяк на ноге.'),
('conversation','burn','Здоровье: объяснить проблему
Пример: I burned my hand.','
Перевод: Я обжёг руку.'),
('conversation','swollen','Здоровье: объяснить проблему
Пример: My ankle is swollen.','
Перевод: У меня отёк лодыжки.'),
('conversation','dizzy','Здоровье: объяснить проблему
Пример: I feel dizzy.','
Перевод: Я чувствую головокружение.'),
('conversation','faint','Здоровье: объяснить проблему
Пример: I feel like I might faint.','
Перевод: Мне кажется, я могу упасть в обморок.'),
('conversation','vomit','Здоровье: объяснить проблему
Пример: I vomited this morning.','
Перевод: Я сегодня утром рвало.'),
('conversation','diarrhea','Здоровье: объяснить проблему
Пример: I have had diarrhea since yesterday.','
Перевод: У меня диарея с вчерашнего дня.'),
('conversation','constipation','Здоровье: объяснить проблему
Пример: I have constipation.','
Перевод: У меня запор.'),
('conversation','pregnant','Здоровье: объяснить проблему
Пример: I am pregnant.','
Перевод: Я беременна.'),
('conversation','symptom','Здоровье: объяснить проблему
Пример: When did the symptoms start?','
Перевод: Когда начались симптомы?'),
('conversation','treatment','Здоровье: объяснить проблему
Пример: What treatment do I need?','
Перевод: Какое лечение мне нужно?'),
('conversation','test','Здоровье: объяснить проблему
Пример: When will I get the test results?','
Перевод: Когда я получу результаты анализа?'),
('conversation','prescription','Здоровье: объяснить проблему
Пример: Do I need a prescription?','
Перевод: Нужен ли рецепт?'),
('conversation','pill','Здоровье: объяснить проблему
Пример: I take one pill in the morning.','
Перевод: Я принимаю одну таблетку утром.'),
('conversation','emergency','Безопасность и помощь
Пример: This is an emergency.','
Перевод: Это чрезвычайная ситуация.'),
('conversation','ambulance','Безопасность и помощь
Пример: Call an ambulance.','
Перевод: Вызовите скорую помощь.'),
('conversation','police','Безопасность и помощь
Пример: I need to call the police.','
Перевод: Мне нужно вызвать полицию.'),
('conversation','fire','Безопасность и помощь
Пример: There is a fire upstairs.','
Перевод: На верхнем этаже пожар.'),
('conversation','smoke','Безопасность и помощь
Пример: I can smell smoke.','
Перевод: Я чувствую запах дыма.'),
('conversation','danger','Безопасность и помощь
Пример: Are we in danger?','
Перевод: Мы в опасности?'),
('conversation','warning','Безопасность и помощь
Пример: Read the warning first.','
Перевод: Сначала прочитайте предупреждение.'),
('conversation','sign','Безопасность и помощь
Пример: Follow the exit signs.','
Перевод: Следуйте указателям выхода.'),
('conversation','rule','Безопасность и помощь
Пример: What are the rules?','
Перевод: Какие правила?'),
('conversation','law','Безопасность и помощь
Пример: It is against the law.','
Перевод: Это противозаконно.'),
('conversation','steal','Безопасность и помощь
Пример: Someone stole my bag.','
Перевод: У меня украли сумку.'),
('conversation','theft','Безопасность и помощь
Пример: I need to report a theft.','
Перевод: Мне нужно сообщить о краже.'),
('conversation','rob','Безопасность и помощь
Пример: He was robbed near the station.','
Перевод: У него ограбили рядом со станцией.'),
('conversation','lost','Безопасность и помощь
Пример: I am lost.','
Перевод: Я потерялся.'),
('conversation','broken','Безопасность и помощь
Пример: The lock is broken.','
Перевод: Замок сломан.'),
('conversation','missing','Безопасность и помощь
Пример: My passport is missing.','
Перевод: Мой паспорт пропал.'),
('conversation','report','Безопасность и помощь
Пример: I want to report a problem.','
Перевод: Я хочу сообщить о проблеме.'),
('conversation','protect','Безопасность и помощь
Пример: Protect your skin from the sun.','
Перевод: Защищайте кожу от солнца.'),
('conversation','avoid','Безопасность и помощь
Пример: Avoid this road at night.','
Перевод: Избегайте этой дороги ночью.'),
('conversation','prevent','Безопасность и помощь
Пример: This helps prevent accidents.','
Перевод: Это помогает предотвратить несчастные случаи.'),
('conversation','escape','Безопасность и помощь
Пример: Everyone escaped safely.','
Перевод: Все благополучно спаслись.'),
('conversation','rescue','Безопасность и помощь
Пример: The firefighters rescued everyone.','
Перевод: Пожарные спасли всех.'),
('conversation','contact','Безопасность и помощь
Пример: Who should I contact?','
Перевод: К кому мне обратиться?'),
('conversation','urgent','Безопасность и помощь
Пример: I need urgent help.','
Перевод: Мне нужна срочная помощь.'),
('conversation','necessary','Безопасность и помощь
Пример: Is this really necessary?','
Перевод: Это действительно необходимо?'),
('conversation','responsible','Безопасность и помощь
Пример: Who is responsible for this?','
Перевод: Кто за это отвечает?'),
('conversation','carefully','Безопасность и помощь
Пример: Read the instructions carefully.','
Перевод: Внимательно прочитайте инструкции.'),
('conversation','safely','Безопасность и помощь
Пример: We arrived safely.','
Перевод: Мы прибыли благополучно.'),
('conversation','helmet','Безопасность и помощь
Пример: Wear a helmet when cycling.','
Перевод: Носите шлем, когда катаетесь на велосипеде.'),
('conversation','seatbelt','Безопасность и помощь
Пример: Please fasten your seatbelt.','
Перевод: Пожалуйста, пристегните ремень безопасности.'),
('conversation','rent','Аренда жилья и коммунальные услуги
Пример: How much is the rent?','
Перевод: Сколько стоит аренда?'),
('conversation','landlord','Аренда жилья и коммунальные услуги
Пример: I need to call the landlord.','
Перевод: Мне нужно позвонить арендодателю.'),
('conversation','tenant','Аренда жилья и коммунальные услуги
Пример: The tenant pays for electricity.','
Перевод: Арендатор платит за электроэнергию.'),
('conversation','lease','Аренда жилья и коммунальные услуги
Пример: When does the lease end?','
Перевод: Когда заканчивается договор аренды?'),
('conversation','deposit','Аренда жилья и коммунальные услуги
Пример: How much is the deposit?','
Перевод: Какой размер залога?'),
('conversation','contract','Аренда жилья и коммунальные услуги
Пример: Read the contract before signing.','
Перевод: Прочитайте договор перед подписанием.'),
('conversation','heating','Аренда жилья и коммунальные услуги
Пример: The heating is not working.','
Перевод: Отопление не работает.'),
('conversation','electricity','Аренда жилья и коммунальные услуги
Пример: Is electricity included?','
Перевод: Включено ли электричество?'),
('conversation','gas','Аренда жилья и коммунальные услуги
Пример: I can smell gas.','
Перевод: Я чувствую запах газа.'),
('conversation','service','Аренда жилья и коммунальные услуги
Пример: Thank you for the good service.','
Перевод: Спасибо за хорошее обслуживание.'),
('conversation','leak','Аренда жилья и коммунальные услуги
Пример: There is a leak in the bathroom.','
Перевод: В ванной течёт вода.'),
('conversation','pipe','Аренда жилья и коммунальные услуги
Пример: A pipe is leaking.','
Перевод: Труба протекает.'),
('conversation','socket','Аренда жилья и коммунальные услуги
Пример: Is there a socket near the bed?','
Перевод: Есть ли розетка рядом с кроватью?'),
('conversation','switch','Аренда жилья и коммунальные услуги
Пример: Where is the light switch?','
Перевод: Где выключатель света?'),
('conversation','meter','Аренда жилья и коммунальные услуги
Пример: Please read the electricity meter.','
Перевод: Пожалуйста, снимите показания электросчётчика.'),
('conversation','stairs','Аренда жилья и коммунальные услуги
Пример: Take the stairs to the second floor.','
Перевод: Поднимитесь по лестнице на второй этаж.'),
('conversation','elevator','Аренда жилья и коммунальные услуги
Пример: The elevator is not working.','
Перевод: Лифт не работает.'),
('conversation','balcony','Аренда жилья и коммунальные услуги
Пример: The room has a balcony.','
Перевод: В комнате есть балкон.'),
('conversation','garden','Аренда жилья и коммунальные услуги
Пример: The children are in the garden.','
Перевод: Дети находятся в саду.'),
('conversation','yard','Аренда жилья и коммунальные услуги
Пример: The dog is in the yard.','
Перевод: Собака находится во дворе.'),
('conversation','garage','Аренда жилья и коммунальные услуги
Пример: The car is in the garage.','
Перевод: Машина в гараже.'),
('conversation','entrance','Аренда жилья и коммунальные услуги
Пример: Use the main entrance.','
Перевод: Используйте главный вход.'),
('conversation','building','Аренда жилья и коммунальные услуги
Пример: Which building do you work in?','
Перевод: В каком здании вы работаете?'),
('conversation','area','Аренда жилья и коммунальные услуги
Пример: Is this a quiet area?','
Перевод: Это тихий район?'),
('conversation','neighborhood','Аренда жилья и коммунальные услуги
Пример: I like this neighborhood.','
Перевод: Мне нравится этот район.'),
('conversation','upstairs','Аренда жилья и коммунальные услуги
Пример: The bathroom is upstairs.','
Перевод: Ванная находится наверху.'),
('conversation','downstairs','Аренда жилья и коммунальные услуги
Пример: I will wait downstairs.','
Перевод: Я подожду внизу.'),
('conversation','apply','Работа: устроиться и договориться
Пример: How do I apply for this job?','
Перевод: Как подать заявку на эту работу?'),
('conversation','application','Работа: устроиться и договориться
Пример: I sent my application yesterday.','
Перевод: Я отправил своё заявление вчера.'),
('conversation','interview','Работа: устроиться и договориться
Пример: I have a job interview tomorrow.','
Перевод: У меня завтра собеседование.'),
('conversation','resume','Работа: устроиться и договориться
Пример: I attached my resume.','
Перевод: Я приложил резюме.'),
('conversation','qualification','Работа: устроиться и договориться
Пример: What qualifications do I need?','
Перевод: Какая квалификация мне нужна?'),
('conversation','degree','Работа: устроиться и договориться
Пример: I have a degree in engineering.','
Перевод: У меня степень инженера.'),
('conversation','certificate','Работа: устроиться и договориться
Пример: I need a copy of my certificate.','
Перевод: Мне нужна копия моего сертификата.'),
('conversation','reference','Работа: устроиться и договориться
Пример: Can you provide a reference?','
Перевод: Можете предоставить рекомендацию?'),
('conversation','position','Работа: устроиться и договориться
Пример: Is this position still available?','
Перевод: Эта вакансия ещё открыта?'),
('conversation','vacancy','Работа: устроиться и договориться
Пример: Are there any vacancies?','
Перевод: Есть ли свободные места?'),
('conversation','hire','Работа: устроиться и договориться
Пример: Are you hiring?','
Перевод: Вы набираете сотрудников?'),
('conversation','employ','Работа: устроиться и договориться
Пример: The company employs fifty people.','
Перевод: В компании работает пятьдесят человек.'),
('conversation','earn','Работа: устроиться и договориться
Пример: How much can I earn?','
Перевод: Сколько я могу зарабатывать?'),
('conversation','wage','Работа: устроиться и договориться
Пример: What is the hourly wage?','
Перевод: Какова почасовая ставка?'),
('conversation','income','Работа: устроиться и договориться
Пример: My income changes each month.','
Перевод: Мой доход меняется каждый месяц.'),
('conversation','tax','Работа: устроиться и договориться
Пример: Is tax included?','
Перевод: Налог включён?'),
('conversation','pension','Работа: устроиться и договориться
Пример: He receives a small pension.','
Перевод: Он получает небольшую пенсию.'),
('conversation','insurance','Работа: устроиться и договориться
Пример: Do I need health insurance?','
Перевод: Нужна ли мне медицинская страховка?'),
('conversation','benefit','Работа: устроиться и договориться
Пример: What benefits does the job offer?','
Перевод: Какие льготы предлагает работа?'),
('conversation','overtime','Работа: устроиться и договориться
Пример: Do you pay for overtime?','
Перевод: Оплачивается ли сверхурочная работа?'),
('conversation','schedule','Работа: устроиться и договориться
Пример: Can we change the schedule?','
Перевод: Можно ли изменить график?'),
('conversation','deadline','Работа: устроиться и договориться
Пример: What is the deadline?','
Перевод: Какой срок?'),
('conversation','available','Работа: устроиться и договориться
Пример: I am available on Monday.','
Перевод: Я доступен в понедельник.'),
('conversation','flexible','Работа: устроиться и договориться
Пример: Are the working hours flexible?','
Перевод: Гибкий ли рабочий график?'),
('conversation','permanent','Работа: устроиться и договориться
Пример: I am looking for permanent work.','
Перевод: Я ищу постоянную работу.'),
('conversation','temporary','Работа: устроиться и договориться
Пример: This is a temporary job.','
Перевод: Это временная работа.'),
('conversation','remote','Работа: устроиться и договориться
Пример: I am looking for remote work.','
Перевод: Я ищу удалённую работу.'),
('conversation','full-time','Работа: устроиться и договориться
Пример: I work full-time.','
Перевод: Я работаю полный рабочий день.'),
('conversation','part-time','Работа: устроиться и договориться
Пример: I work part-time.','
Перевод: Я работаю неполный рабочий день.'),
('conversation','retire','Работа: устроиться и договориться
Пример: My father will retire next year.','
Перевод: Мой отец выйдет на пенсию в следующем году.'),
('conversation','arrange','Договариваться и решать задачи
Пример: Can we arrange a meeting?','
Перевод: Можно ли назначить встречу?'),
('conversation','confirm','Договариваться и решать задачи
Пример: Please confirm the date.','
Перевод: Пожалуйста, подтвердите дату.'),
('conversation','postpone','Договариваться и решать задачи
Пример: Can we postpone the meeting?','
Перевод: Можно перенести встречу?'),
('conversation','reschedule','Договариваться и решать задачи
Пример: I need to reschedule my appointment.','
Перевод: Мне нужно перенести мою запись.'),
('conversation','suggest','Договариваться и решать задачи
Пример: What do you suggest?','
Перевод: Что вы предлагаете?'),
('conversation','recommend','Договариваться и решать задачи
Пример: Can you recommend a good hotel?','
Перевод: Можете порекомендовать хороший отель?'),
('conversation','advice','Договариваться и решать задачи
Пример: Thank you for your advice.','
Перевод: Спасибо за ваш совет.'),
('conversation','advise','Договариваться и решать задачи
Пример: What would you advise me to do?','
Перевод: Что бы вы посоветовали мне сделать?'),
('conversation','request','Договариваться и решать задачи
Пример: I have a request.','
Перевод: У меня есть запрос.'),
('conversation','require','Договариваться и решать задачи
Пример: Does this job require experience?','
Перевод: Требуется ли опыт для этой работы?'),
('conversation','provide','Договариваться и решать задачи
Пример: We provide the equipment.','
Перевод: Мы предоставляем оборудование.'),
('conversation','deliver','Договариваться и решать задачи
Пример: Can you deliver it tomorrow?','
Перевод: Можете доставить это завтра?'),
('conversation','delivery','Договариваться и решать задачи
Пример: Is delivery included?','
Перевод: Включена ли доставка?'),
('conversation','supply','Договариваться и решать задачи
Пример: There is a problem with the water supply.','
Перевод: Есть проблема с водоснабжением.'),
('conversation','organize','Договариваться и решать задачи
Пример: Who will organize the meeting?','
Перевод: Кто будет организовывать встречу?'),
('conversation','complete','Договариваться и решать задачи
Пример: Please complete this form.','
Перевод: Пожалуйста, заполните эту форму.'),
('conversation','achieve','Договариваться и решать задачи
Пример: I want to achieve my goal.','
Перевод: Я хочу достичь своей цели.'),
('conversation','solve','Договариваться и решать задачи
Пример: Let''s solve this together.','
Перевод: Давайте решим это вместе.'),
('conversation','deal','Договариваться и решать задачи
Пример: We need to deal with this problem.','
Перевод: Нужно решить эту проблему.'),
('conversation','handle','Договариваться и решать задачи
Пример: I can handle this task.','
Перевод: Я могу справиться с этой задачей.'),
('conversation','depend','Договариваться и решать задачи
Пример: It depends on the weather.','
Перевод: Это зависит от погоды.'),
('conversation','matter','Договариваться и решать задачи
Пример: It does not matter.','
Перевод: Это не имеет значения.'),
('conversation','involve','Договариваться и решать задачи
Пример: What does the job involve?','
Перевод: В чём заключается работа?'),
('conversation','consider','Договариваться и решать задачи
Пример: I will consider your offer.','
Перевод: Я рассмотрю ваше предложение.'),
('conversation','compare','Договариваться и решать задачи
Пример: Let''s compare the prices.','
Перевод: Давайте сравним цены.'),
('conversation','detail','Договариваться и решать задачи
Пример: Can you send me the details?','
Перевод: Можете прислать мне детали?'),
('conversation','option','Договариваться и решать задачи
Пример: What are my options?','
Перевод: Каковы мои варианты?'),
('conversation','choice','Договариваться и решать задачи
Пример: It is your choice.','
Перевод: Это ваш выбор.'),
('conversation','useful','Качество, удобство и мнение
Пример: This is very useful.','
Перевод: Это очень полезно.'),
('conversation','useless','Качество, удобство и мнение
Пример: This charger is useless without a cable.','
Перевод: Этот зарядный адаптер бесполезен без кабеля.'),
('conversation','helpful','Качество, удобство и мнение
Пример: The staff were very helpful.','
Перевод: Персонал был очень полезен.'),
('conversation','suitable','Качество, удобство и мнение
Пример: Is this suitable for children?','
Перевод: Это подходит для детей?'),
('conversation','convenient','Качество, удобство и мнение
Пример: What time is convenient for you?','
Перевод: Во сколько вам удобно?'),
('conversation','reliable','Качество, удобство и мнение
Пример: The bus service is reliable.','
Перевод: Автобусное сообщение надёжно.'),
('conversation','efficient','Качество, удобство и мнение
Пример: This is a more efficient way.','
Перевод: Это более эффективный способ.'),
('conversation','practical','Качество, удобство и мнение
Пример: I need practical advice.','
Перевод: Мне нужен практический совет.'),
('conversation','basic','Качество, удобство и мнение
Пример: I know some basic English.','
Перевод: Я знаю базовый английский.'),
('conversation','advanced','Качество, удобство и мнение
Пример: This course is too advanced for me.','
Перевод: Этот курс слишком продвинутый для меня.'),
('conversation','difficult','Качество, удобство и мнение
Пример: This is difficult to explain.','
Перевод: Это трудно объяснить.'),
('conversation','complex','Качество, удобство и мнение
Пример: It is a complex problem.','
Перевод: Это сложная проблема.'),
('conversation','similar','Качество, удобство и мнение
Пример: These two words are similar.','
Перевод: Эти два слова похожи.'),
('conversation','equal','Качество, удобство и мнение
Пример: Divide it into equal parts.','
Перевод: Разделите это на равные части.'),
('conversation','main','Качество, удобство и мнение
Пример: What is the main problem?','
Перевод: В чём главная проблема?'),
('conversation','whole','Качество, удобство и мнение
Пример: I waited the whole day.','
Перевод: Я ждал весь день.'),
('conversation','entire','Качество, удобство и мнение
Пример: The entire family came.','
Перевод: Всю семью пришли.'),
('conversation','particular','Качество, удобство и мнение
Пример: Do you have a particular time in mind?','
Перевод: У вас есть конкретное время в виду?'),
('conversation','general','Качество, удобство и мнение
Пример: I have a general question.','
Перевод: У меня общий вопрос.'),
('conversation','usual','Качество, удобство и мнение
Пример: I will have my usual coffee.','
Перевод: Я возьму свой обычный кофе.'),
('conversation','unusual','Качество, удобство и мнение
Пример: That is unusual for him.','
Перевод: Это для него необычно.'),
('conversation','popular','Качество, удобство и мнение
Пример: This is a popular place.','
Перевод: Это популярное место.'),
('conversation','famous','Качество, удобство и мнение
Пример: The city is famous for its food.','
Перевод: Город известен своей кухней.'),
('conversation','perfect','Качество, удобство и мнение
Пример: That time is perfect for me.','
Перевод: Это время идеально для меня.'),
('conversation','excellent','Качество, удобство и мнение
Пример: The service was excellent.','
Перевод: Обслуживание было отличным.'),
('conversation','awful','Качество, удобство и мнение
Пример: The weather was awful.','
Перевод: Погода была ужасной.'),
('conversation','terrible','Качество, удобство и мнение
Пример: I had a terrible headache.','
Перевод: У меня была ужасная головная боль.'),
('conversation','wonderful','Качество, удобство и мнение
Пример: We had a wonderful time.','
Перевод: Мы прекрасно провели время.'),
('conversation','amazing','Качество, удобство и мнение
Пример: The view is amazing.','
Перевод: Вид потрясающий.'),
('conversation','opinion','Слова для собственного мнения
Пример: What is your opinion?','
Перевод: Каково ваше мнение?'),
('conversation','point','Слова для собственного мнения
Пример: I understand your point.','
Перевод: Я понимаю вашу точку зрения.'),
('conversation','view','Слова для собственного мнения
Пример: What is your view on this?','
Перевод: Каково ваше мнение по этому поводу?'),
('conversation','fact','Слова для собственного мнения
Пример: That is a fact.','
Перевод: Это факт.'),
('conversation','truth','Слова для собственного мнения
Пример: Tell me the truth.','
Перевод: Скажите правду.'),
('conversation','lie','Слова для собственного мнения
Пример: Please do not lie to me.','
Перевод: Пожалуйста, не лгите мне.'),
('conversation','doubt','Слова для собственного мнения
Пример: I have some doubts.','
Перевод: У меня есть сомнения.'),
('conversation','mistake','Слова для собственного мнения
Пример: I made a mistake.','
Перевод: Я совершил ошибку.'),
('conversation','error','Слова для собственного мнения
Пример: There is an error in the bill.','
Перевод: В счёте есть ошибка.'),
('conversation','chance','Слова для собственного мнения
Пример: Give me a chance.','
Перевод: Дайте мне шанс.'),
('conversation','opportunity','Слова для собственного мнения
Пример: Thank you for this opportunity.','
Перевод: Спасибо за эту возможность.'),
('conversation','possibility','Слова для собственного мнения
Пример: Is there a possibility of a refund?','
Перевод: Есть ли возможность возврата?'),
('conversation','decision','Слова для собственного мнения
Пример: It was a difficult decision.','
Перевод: Это было трудное решение.'),
('conversation','purpose','Слова для собственного мнения
Пример: What is the purpose of this form?','
Перевод: Какова цель этой формы?'),
('conversation','cause','Слова для собственного мнения
Пример: What caused the delay?','
Перевод: Что стало причиной задержки?'),
('conversation','effect','Слова для собственного мнения
Пример: What effect will this have?','
Перевод: Каков будет результат?'),
('conversation','difference','Слова для собственного мнения
Пример: What is the difference?','
Перевод: В чём разница?'),
('conversation','advantage','Слова для собственного мнения
Пример: What is the main advantage?','
Перевод: Каково главное преимущество?'),
('conversation','disadvantage','Слова для собственного мнения
Пример: The main disadvantage is the price.','
Перевод: Главный недостаток — цена.'),
('conversation','risk','Слова для собственного мнения
Пример: Is there any risk?','
Перевод: Есть ли какой‑то риск?'),
('conversation','success','Слова для собственного мнения
Пример: The event was a success.','
Перевод: Мероприятие прошло успешно.'),
('conversation','failure','Слова для собственного мнения
Пример: Failure is part of learning.','
Перевод: Неудача — часть обучения.'),
('conversation','trouble','Слова для собственного мнения
Пример: I am having trouble with this.','
Перевод: У меня возникли проблемы с этим.'),
('conversation','issue','Слова для собственного мнения
Пример: There is an issue with my payment.','
Перевод: Есть проблема с моим платежом.'),
('conversation','situation','Слова для собственного мнения
Пример: Let me explain the situation.','
Перевод: Позвольте объяснить ситуацию.'),
('conversation','condition','Слова для собственного мнения
Пример: The car is in good condition.','
Перевод: Автомобиль в хорошем состоянии.'),
('conversation','case','Слова для собственного мнения
Пример: In that case, let''s wait.','
Перевод: В таком случае подождём.'),
('conversation','information','Слова для собственного мнения
Пример: I need more information.','
Перевод: Мне нужна дополнительная информация.'),
('conversation','knowledge','Слова для собственного мнения
Пример: I want to improve my knowledge.','
Перевод: Я хочу расширить свои знания.'),
('conversation','shut','Бытовые глаголы и просьбы
Пример: Please shut the gate.','
Перевод: Пожалуйста, закройте ворота.'),
('conversation','press','Бытовые глаголы и просьбы
Пример: Press the green button.','
Перевод: Нажмите зелёную кнопку.'),
('conversation','touch','Бытовые глаголы и просьбы
Пример: Please do not touch that.','
Перевод: Пожалуйста, не трогайте это.'),
('conversation','smell','Бытовые глаголы и просьбы
Пример: This smells good.','
Перевод: Это приятно пахнет.'),
('conversation','sound','Бытовые глаголы и просьбы
Пример: That sounds like a good plan.','
Перевод: Звучит как хороший план.'),
('conversation','appear','Бытовые глаголы и просьбы
Пример: A message appeared on the screen.','
Перевод: На экране появилось сообщение.'),
('conversation','disappear','Бытовые глаголы и просьбы
Пример: My keys have disappeared.','
Перевод: Мои ключи исчезли.'),
('conversation','search','Бытовые глаголы и просьбы
Пример: I am searching for a new job.','
Перевод: Я ищу новую работу.'),
('conversation','hide','Бытовые глаголы и просьбы
Пример: Where did you hide the keys?','
Перевод: Где ты спрятал ключи?'),
('conversation','discover','Бытовые глаголы и просьбы
Пример: We discovered a nice cafe.','
Перевод: Мы нашли хороший кафе.'),
('conversation','pack','Бытовые глаголы и просьбы
Пример: I need to pack my bag.','
Перевод: Мне нужно собрать вещи в сумку.'),
('conversation','unpack','Бытовые глаголы и просьбы
Пример: Let me unpack first.','
Перевод: Сначала я распакую.'),
('conversation','load','Бытовые глаголы и просьбы
Пример: Help me load the car.','
Перевод: Помогите загрузить машину.'),
('conversation','unload','Бытовые глаголы и просьбы
Пример: We need to unload the boxes.','
Перевод: Нужно выгрузить коробки.'),
('conversation','wrap','Бытовые глаголы и просьбы
Пример: Could you wrap this gift?','
Перевод: Не могли бы вы упаковать этот подарок?'),
('conversation','tie','Бытовые глаголы и просьбы
Пример: Tie your shoes.','
Перевод: Зашнуйте ботинки.'),
('conversation','hang','Бытовые глаголы и просьбы
Пример: Hang your coat here.','
Перевод: Повесьте пальто здесь.'),
('conversation','fold','Бытовые глаголы и просьбы
Пример: Fold the clothes, please.','
Перевод: Сложите, пожалуйста, одежду.'),
('conversation','iron','Бытовые глаголы и просьбы
Пример: I need to iron my shirt.','
Перевод: Мне нужно погладить рубашку.'),
('conversation','sweep','Бытовые глаголы и просьбы
Пример: Sweep the floor, please.','
Перевод: Подметите пол, пожалуйста.'),
('conversation','wipe','Бытовые глаголы и просьбы
Пример: Wipe the table.','
Перевод: Протрите стол.'),
('conversation','rub','Бытовые глаголы и просьбы
Пример: Do not rub your eyes.','
Перевод: Не трите глаза.'),
('conversation','shake','Бытовые глаголы и просьбы
Пример: They shook hands.','
Перевод: Они пожали друг другу руки.'),
('conversation','knock','Бытовые глаголы и просьбы
Пример: Knock before you enter.','
Перевод: Постучите, прежде чем войти.'),
('conversation','event','Праздники, встречи, общественная жизнь
Пример: What time does the event start?','
Перевод: Во сколько начинается мероприятие?'),
('conversation','celebration','Праздники, встречи, общественная жизнь
Пример: We are having a small celebration.','
Перевод: Мы отмечаем небольшое торжество.'),
('conversation','invitation','Праздники, встречи, общественная жизнь
Пример: Thank you for the invitation.','
Перевод: Спасибо за приглашение.'),
('conversation','anniversary','Праздники, встречи, общественная жизнь
Пример: It is our wedding anniversary.','
Перевод: Это наша годовщина свадьбы.'),
('conversation','festival','Праздники, встречи, общественная жизнь
Пример: There is a music festival this weekend.','
Перевод: В эти выходные будет музыкальный фестиваль.'),
('conversation','concert','Праздники, встречи, общественная жизнь
Пример: We went to a concert.','
Перевод: Мы ходили на концерт.'),
('conversation','theater','Праздники, встречи, общественная жизнь
Пример: I booked tickets for the theater.','
Перевод: Я забронировал билеты в театр.'),
('conversation','cinema','Праздники, встречи, общественная жизнь
Пример: Let''s go to the cinema.','
Перевод: Пойдём в кино.'),
('conversation','museum','Праздники, встречи, общественная жизнь
Пример: Is the museum open today?','
Перевод: Сегодня музей открыт?'),
('conversation','library','Праздники, встречи, общественная жизнь
Пример: I borrowed a book from the library.','
Перевод: Я взял книгу из библиотеки.'),
('conversation','club','Праздники, встречи, общественная жизнь
Пример: I joined a sports club.','
Перевод: Я записался в спортивный клуб.'),
('conversation','community','Праздники, встречи, общественная жизнь
Пример: I want to help the local community.','
Перевод: Я хочу помогать местному сообществу.'),
('conversation','volunteer','Праздники, встречи, общественная жизнь
Пример: I volunteer on weekends.','
Перевод: По выходным я работаю волонтёром.'),
('conversation','charity','Праздники, встречи, общественная жизнь
Пример: The money goes to charity.','
Перевод: Деньги идут на благотворительность.'),
('conversation','religion','Праздники, встречи, общественная жизнь
Пример: People here have different religions.','
Перевод: Здесь люди исповедуют разные религии.'),
('conversation','culture','Праздники, встречи, общественная жизнь
Пример: I want to learn about the local culture.','
Перевод: Я хочу узнать о местной культуре.'),
('conversation','tradition','Праздники, встречи, общественная жизнь
Пример: It is a family tradition.','
Перевод: Это семейная традиция.'),
('conversation','custom','Праздники, встречи, общественная жизнь
Пример: It is a local custom.','
Перевод: Это местный обычай.'),
('conversation','freedom','Праздники, встречи, общественная жизнь
Пример: Freedom is important to me.','
Перевод: Свобода важна для меня.'),
('conversation','peace','Праздники, встречи, общественная жизнь
Пример: I just want some peace and quiet.','
Перевод: Я просто хочу тишины и покоя.'),
('conversation','war','Праздники, встречи, общественная жизнь
Пример: The war changed many lives.','
Перевод: Война изменила многие жизни.'),
('conversation','news','Праздники, встречи, общественная жизнь
Пример: Have you heard the news?','
Перевод: Вы слышали новости?'),
('conversation','newspaper','Праздники, встречи, общественная жизнь
Пример: I read the local newspaper.','
Перевод: Я читаю местную газету.'),
('conversation','magazine','Праздники, встречи, общественная жизнь
Пример: I bought a travel magazine.','
Перевод: Я купил журнал о путешествиях.'),
('conversation','article','Праздники, встречи, общественная жизнь
Пример: I read an interesting article.','
Перевод: Я прочитал интересную статью.'),
('conversation','story','Праздники, встречи, общественная жизнь
Пример: Tell me your story.','
Перевод: Расскажите свою историю.'),
('conversation','history','Праздники, встречи, общественная жизнь
Пример: I am interested in history.','
Перевод: Я интересуюсь историей.'),
('conversation','future','Праздники, встречи, общественная жизнь
Пример: What are your plans for the future?','
Перевод: Какие у вас планы на будущее?'),
('conversation','past','Праздники, встречи, общественная жизнь
Пример: That happened in the past.','
Перевод: Это случилось в прошлом.'),
('conversation','phrase','Уточнить, описать, переспросить
Пример: How do I say this phrase?','
Перевод: Как сказать эту фразу?'),
('conversation','expression','Уточнить, описать, переспросить
Пример: That is a useful expression.','
Перевод: Это полезное выражение.'),
('conversation','pronunciation','Уточнить, описать, переспросить
Пример: I want to improve my pronunciation.','
Перевод: Я хочу улучшить своё произношение.'),
('conversation','accent','Уточнить, описать, переспросить
Пример: Your accent is easy to understand.','
Перевод: Ваш акцент легко понять.'),
('conversation','grammar','Уточнить, описать, переспросить
Пример: I need help with grammar.','
Перевод: Мне нужна помощь с грамматикой.'),
('conversation','vocabulary','Уточнить, описать, переспросить
Пример: I am building my vocabulary.','
Перевод: Я расширяю свой словарный запас.'),
('conversation','fluent','Уточнить, описать, переспросить
Пример: I want to become fluent in English.','
Перевод: Я хочу свободно говорить по‑английски.'),
('conversation','clearly','Уточнить, описать, переспросить
Пример: Please speak clearly.','
Перевод: Пожалуйста, говорите чётко.'),
('conversation','briefly','Уточнить, описать, переспросить
Пример: Can you explain it briefly?','
Перевод: Не могли бы вы объяснить это вкратце?'),
('conversation','aloud','Уточнить, описать, переспросить
Пример: Read the sentence aloud.','
Перевод: Прочитайте предложение вслух.'),
('conversation','separately','Уточнить, описать, переспросить
Пример: Can we pay separately?','
Перевод: Можно оплатить отдельно?'),
('conversation','directly','Уточнить, описать, переспросить
Пример: Please speak to me directly.','
Перевод: Пожалуйста, говорите со мной напрямую.'),
('conversation','properly','Уточнить, описать, переспросить
Пример: The door does not close properly.','
Перевод: Дверь не закрывается должным образом.'),
('conversation','correctly','Уточнить, описать, переспросить
Пример: Did I pronounce that correctly?','
Перевод: Я правильно произнёс?'),
('conversation','completely','Уточнить, описать, переспросить
Пример: I completely agree.','
Перевод: Я полностью согласен.'),
('conversation','partly','Уточнить, описать, переспросить
Пример: That is partly true.','
Перевод: Это отчасти верно.'),
('conversation','mostly','Уточнить, описать, переспросить
Пример: I mostly work from home.','
Перевод: Я в основном работаю из дома.'),
('conversation','mainly','Уточнить, описать, переспросить
Пример: We mainly sell food.','
Перевод: Мы в основном продаём еду.'),
('conversation','nearly','Уточнить, описать, переспросить
Пример: I am nearly ready.','
Перевод: Я почти готов.'),
('conversation','rather','Уточнить, описать, переспросить
Пример: I would rather stay home.','
Перевод: Я бы предпочёл остаться дома.'),
('conversation','absolutely','Уточнить, описать, переспросить
Пример: You are absolutely right.','
Перевод: Вы абсолютно правы.'),
('conversation','certainly','Уточнить, описать, переспросить
Пример: I can certainly help.','
Перевод: Я, конечно, помогу.'),
('conversation','possibly','Уточнить, описать, переспросить
Пример: Could you possibly help me?','
Перевод: Не могли бы вы помочь мне?'),
('conversation','hardly','Уточнить, описать, переспросить
Пример: I can hardly hear you.','
Перевод: Я еле слышу вас.'),
('conversation','particularly','Уточнить, описать, переспросить
Пример: I particularly like this one.','
Перевод: Мне особенно нравится этот вариант.'),
('conversation','slightly','Уточнить, описать, переспросить
Пример: This one is slightly cheaper.','
Перевод: Этот вариант немного дешевле.'),
('conversation','document','Документы и переезд
Пример: Bring your original documents.','
Перевод: Принесите оригиналы документов.'),
('conversation','form','Документы и переезд
Пример: Please fill in this form.','
Перевод: Пожалуйста, заполните эту форму.'),
('conversation','signature','Документы и переезд
Пример: We need your signature here.','
Перевод: Нам нужна ваша подпись здесь.'),
('conversation','stamp','Документы и переезд
Пример: Do I need a stamp?','
Перевод: Нужна ли печать?'),
('conversation','original','Документы и переезд
Пример: Please bring the original.','
Перевод: Пожалуйста, принесите оригинал.'),
('conversation','photocopy','Документы и переезд
Пример: I need a photocopy of this.','
Перевод: Мне нужна копия этого документа.'),
('conversation','identity','Документы и переезд
Пример: They need to check my identity.','
Перевод: Им нужно проверить мою личность.'),
('conversation','citizen','Документы и переезд
Пример: I am a citizen of this country.','
Перевод: Я гражданин этой страны.'),
('conversation','citizenship','Документы и переезд
Пример: How do I apply for citizenship?','
Перевод: Как подать заявление на гражданство?'),
('conversation','visa','Документы и переезд
Пример: Do I need a visa?','
Перевод: Нужна ли виза?'),
('conversation','permit','Документы и переезд
Пример: I need a work permit.','
Перевод: Мне нужен разрешение на работу.'),
('conversation','residence','Документы и переезд
Пример: I need proof of residence.','
Перевод: Мне нужен документ, подтверждающий место жительства.'),
('conversation','resident','Документы и переезд
Пример: This discount is for local residents.','
Перевод: Эта скидка предназначена для местных жителей.'),
('conversation','border','Документы и переезд
Пример: We crossed the border yesterday.','
Перевод: Мы пересекли границу вчера.'),
('conversation','customs','Документы и переезд
Пример: We went through customs.','
Перевод: Мы прошли таможню.'),
('conversation','embassy','Документы и переезд
Пример: I need to contact my embassy.','
Перевод: Мне нужно связаться с посольством.'),
('conversation','consulate','Документы и переезд
Пример: Where is the nearest consulate?','
Перевод: Где находится ближайшее консульство?'),
('conversation','registration','Документы и переезд
Пример: Is registration required?','
Перевод: Требуется ли регистрация?'),
('conversation','register','Документы и переезд
Пример: How do I register?','
Перевод: Как зарегистрироваться?'),
('conversation','renew','Документы и переезд
Пример: I need to renew my passport.','
Перевод: Мне нужно обновить паспорт.'),
('conversation','expire','Документы и переезд
Пример: When does your visa expire?','
Перевод: Когда истекает срок действия вашей визы?'),
('conversation','valid','Документы и переезд
Пример: Is this ticket still valid?','
Перевод: Этот билет ещё действителен?'),
('conversation','proof','Документы и переезд
Пример: Do you need proof of address?','
Перевод: Вам нужен документ, подтверждающий адрес?'),
('conversation','evidence','Документы и переезд
Пример: Do you have any evidence?','
Перевод: У вас есть какие‑либо доказательства?'),
('conversation','official','Документы и переезд
Пример: I need an official translation.','
Перевод: Мне нужен официальный перевод.'),
('conversation','legal','Документы и переезд
Пример: Is it legal to park here?','
Перевод: Можно ли здесь парковаться?'),
('conversation','illegal','Документы и переезд
Пример: That is illegal here.','
Перевод: Это здесь запрещено законом.'),
('conversation','lawyer','Документы и переезд
Пример: I need to speak to a lawyer.','
Перевод: Мне нужно поговорить с юристом.'),
('conversation','fee','Документы и переезд
Пример: What is the application fee?','
Перевод: Какова плата за подачу заявления?'),
('conversation','payment','Банк, платежи и счета
Пример: My payment did not go through.','
Перевод: Мой платёж не прошёл.'),
('conversation','transfer','Банк, платежи и счета
Пример: I need to transfer money.','
Перевод: Мне нужно перевести деньги.'),
('conversation','withdraw','Банк, платежи и счета
Пример: Where can I withdraw cash?','
Перевод: Где можно снять наличные?'),
('conversation','withdrawal','Банк, платежи и счета
Пример: Is there a withdrawal fee?','
Перевод: Есть ли комиссия за снятие?'),
('conversation','credit','Банк, платежи и счета
Пример: Can I pay by credit card?','
Перевод: Можно ли оплатить кредитной картой?'),
('conversation','debit','Банк, платежи и счета
Пример: I use a debit card.','
Перевод: Я использую дебетовую карту.'),
('conversation','debt','Банк, платежи и счета
Пример: I want to pay off my debt.','
Перевод: Я хочу погасить долг.'),
('conversation','loan','Банк, платежи и счета
Пример: I applied for a loan.','
Перевод: Я подал заявку на кредит.'),
('conversation','interest','Банк, платежи и счета
Пример: What is the interest rate?','
Перевод: Какова процентная ставка?'),
('conversation','rate','Банк, платежи и счета
Пример: What is the exchange rate?','
Перевод: Каков курс обмена?'),
('conversation','currency','Банк, платежи и счета
Пример: Can I exchange foreign currency here?','
Перевод: Можно ли здесь обменять иностранную валюту?'),
('conversation','budget','Банк, платежи и счета
Пример: This is outside my budget.','
Перевод: Это выходит за пределы моего бюджета.'),
('conversation','expense','Банк, платежи и счета
Пример: Rent is my biggest expense.','
Перевод: Аренда – моя самая большая статья расходов.'),
('conversation','savings','Банк, платежи и счета
Пример: I am using my savings.','
Перевод: Я использую свои сбережения.'),
('conversation','profit','Банк, платежи и счета
Пример: The business made a small profit.','
Перевод: Бизнес получил небольшую прибыль.'),
('conversation','loss','Банк, платежи и счета
Пример: They reported a loss.','
Перевод: Они сообщили о убытке.'),
('conversation','statement','Банк, платежи и счета
Пример: I need a bank statement.','
Перевод: Мне нужна банковская выписка.'),
('conversation','invoice','Банк, платежи и счета
Пример: Please send me an invoice.','
Перевод: Пожалуйста, пришлите мне счёт‑фактуру.'),
('conversation','due','Банк, платежи и счета
Пример: When is the payment due?','
Перевод: Когда срок оплаты?'),
('conversation','overdue','Банк, платежи и счета
Пример: This bill is overdue.','
Перевод: Этот счёт просрочен.'),
('conversation','owe','Банк, платежи и счета
Пример: How much do I owe you?','
Перевод: Сколько я вам должен?'),
('conversation','split','Банк, платежи и счета
Пример: Can we split the bill?','
Перевод: Можно разделить счёт?'),
('conversation','tip','Банк, платежи и счета
Пример: Is the tip included?','
Перевод: Включён ли сервисный сбор?'),
('conversation','subscription','Банк, платежи и счета
Пример: I want to cancel my subscription.','
Перевод: Я хочу отменить подписку.'),
('conversation','automatic','Банк, платежи и счета
Пример: Can I set up automatic payments?','
Перевод: Можно настроить автоматические платежи?'),
('conversation','vehicle','Машина и обслуживание
Пример: No vehicles are allowed here.','
Перевод: Здесь запрещено движение транспортных средств.'),
('conversation','engine','Машина и обслуживание
Пример: The engine will not start.','
Перевод: Двигатель не заводится.'),
('conversation','wheel','Машина и обслуживание
Пример: Keep both hands on the wheel.','
Перевод: Держите обе руки на руле.'),
('conversation','tire','Машина и обслуживание
Пример: I have a flat tire.','
Перевод: У меня спустило колесо.'),
('conversation','brake','Машина и обслуживание
Пример: The brakes need checking.','
Перевод: Тормоза нуждаются в проверке.'),
('conversation','fuel','Машина и обслуживание
Пример: We need more fuel.','
Перевод: Нужен дополнительный топливо.'),
('conversation','petrol','Машина и обслуживание
Пример: Where can I buy petrol?','
Перевод: Где можно заправиться?'),
('conversation','diesel','Машина и обслуживание
Пример: This car uses diesel.','
Перевод: Этот автомобиль работает на дизеле.'),
('conversation','license','Машина и обслуживание
Пример: Can I see your driving license?','
Перевод: Можно увидеть ваш водительский билет?'),
('conversation','parking','Машина и обслуживание
Пример: Is parking free here?','
Перевод: Здесь бесплатная парковка?'),
('conversation','mechanic','Машина и обслуживание
Пример: The mechanic checked the engine.','
Перевод: Механик проверил двигатель.'),
('conversation','damage','Машина и обслуживание
Пример: There is damage to the door.','
Перевод: Есть повреждение двери.'),
('conversation','fault','Машина и обслуживание
Пример: It is not your fault.','
Перевод: Это не ваша вина.'),
('conversation','scratch','Машина и обслуживание
Пример: There is a scratch on the car.','
Перевод: На машине есть царапина.'),
('conversation','dent','Машина и обслуживание
Пример: There is a dent in the door.','
Перевод: В двери вмятина.'),
('conversation','spare','Машина и обслуживание
Пример: Do you have a spare key?','
Перевод: У вас есть запасной ключ?'),
('conversation','tool','Машина и обслуживание
Пример: I need the right tool.','
Перевод: Мне нужен правильный инструмент.'),
('conversation','equipment','Машина и обслуживание
Пример: Is the equipment included?','
Перевод: Оборудование включено?'),
('conversation','limit','Машина и обслуживание
Пример: What is the speed limit?','
Перевод: Какой ограничение скорости?'),
('conversation','motorway','Машина и обслуживание
Пример: Take the motorway to the airport.','
Перевод: Поезжайте по автомагистрали к аэропорту.'),
('conversation','highway','Машина и обслуживание
Пример: The highway is closed.','
Перевод: Шоссе закрыто.'),
('conversation','roundabout','Машина и обслуживание
Пример: Turn left at the roundabout.','
Перевод: Поверните налево на кольце.'),
('conversation','junction','Машина и обслуживание
Пример: Take the next junction.','
Перевод: Съезжайте на следующем перекрёстке.'),
('conversation','crossing','Машина и обслуживание
Пример: Use the pedestrian crossing.','
Перевод: Используйте пешеходный переход.'),
('conversation','pedestrian','Машина и обслуживание
Пример: Watch out for pedestrians.','
Перевод: Осторожно, пешеходы!'),
('conversation','pavement','Машина и обслуживание
Пример: Walk on the pavement.','
Перевод: Идите по тротуару.'),
('conversation','sidewalk','Путешествие подробнее
Пример: Stay on the sidewalk.','
Перевод: Оставайтесь на тротуаре.'),
('conversation','subway','Путешествие подробнее
Пример: I take the subway to work.','
Перевод: Я езжу на метро на работу.'),
('conversation','underground','Путешествие подробнее
Пример: Where is the underground station?','
Перевод: Где находится станция метро?'),
('conversation','tram','Путешествие подробнее
Пример: The tram stops here.','
Перевод: Трамвай останавливается здесь.'),
('conversation','ferry','Путешествие подробнее
Пример: We took the ferry across the river.','
Перевод: Мы переправились на пароме через реку.'),
('conversation','boat','Путешествие подробнее
Пример: We rented a small boat.','
Перевод: Мы арендовали небольшую лодку.'),
('conversation','ship','Путешествие подробнее
Пример: The ship leaves tonight.','
Перевод: Корабль отплывает сегодня вечером.'),
('conversation','port','Путешествие подробнее
Пример: The port is near the city.','
Перевод: Порт находится рядом с городом.'),
('conversation','terminal','Путешествие подробнее
Пример: Which terminal do we need?','
Перевод: Какой терминал нам нужен?'),
('conversation','gate','Путешествие подробнее
Пример: Our flight leaves from gate ten.','
Перевод: Наш рейс отправляется из выхода десять.'),
('conversation','boarding','Путешествие подробнее
Пример: Boarding starts in twenty minutes.','
Перевод: Посадка начнётся через двадцать минут.'),
('conversation','departure','Путешествие подробнее
Пример: What is the departure time?','
Перевод: Во сколько время вылета?'),
('conversation','arrival','Путешествие подробнее
Пример: Check the arrival time.','
Перевод: Проверьте время прибытия.'),
('conversation','connection','Путешествие подробнее
Пример: I missed my connection.','
Перевод: Я пропустил свою пересадку.'),
('conversation','direct','Путешествие подробнее
Пример: Is there a direct train?','
Перевод: Есть ли прямой поезд?'),
('conversation','route','Путешествие подробнее
Пример: Which route is faster?','
Перевод: Какой маршрут быстрее?'),
('conversation','destination','Путешествие подробнее
Пример: What is your final destination?','
Перевод: Какой ваш конечный пункт назначения?'),
('conversation','reservation','Путешествие подробнее
Пример: I have a reservation.','
Перевод: У меня есть бронь.'),
('conversation','booking','Путешествие подробнее
Пример: Can I change my booking?','
Перевод: Можно изменить бронирование?'),
('conversation','reception','Путешествие подробнее
Пример: Ask at reception.','
Перевод: Обратитесь на ресепшн.'),
('conversation','receptionist','Путешествие подробнее
Пример: The receptionist was helpful.','
Перевод: Ресепшионист был полезен.'),
('conversation','checkout','Путешествие подробнее
Пример: What time is checkout?','
Перевод: Во сколько время выезда?'),
('conversation','included','Путешествие подробнее
Пример: Is breakfast included?','
Перевод: Завтрак включён?'),
('conversation','tour','Путешествие подробнее
Пример: We booked a city tour.','
Перевод: Мы забронировали экскурсию по городу.'),
('conversation','guide','Путешествие подробнее
Пример: Our guide speaks English.','
Перевод: Наш гид говорит по‑английски.'),
('conversation','tourist','Путешествие подробнее
Пример: Is there a tourist information office?','
Перевод: Есть ли туристический информационный пункт?'),
('conversation','sightseeing','Путешествие подробнее
Пример: We went sightseeing yesterday.','
Перевод: Вчера мы ходили осматривать достопримечательности.'),
('conversation','education','Учёба и развитие
Пример: Education is important to me.','
Перевод: Образование важно для меня.'),
('conversation','university','Учёба и развитие
Пример: She studies at university.','
Перевод: Она учится в университете.'),
('conversation','college','Учёба и развитие
Пример: I am applying to college.','
Перевод: Я подаю документы в колледж.'),
('conversation','subject','Учёба и развитие
Пример: What is your favorite subject?','
Перевод: Какой ваш любимый предмет?'),
('conversation','exam','Учёба и развитие
Пример: I have an exam tomorrow.','
Перевод: У меня экзамен завтра.'),
('conversation','examination','Учёба и развитие
Пример: The examination lasts two hours.','
Перевод: Экзамен длится два часа.'),
('conversation','mark','Учёба и развитие
Пример: I got a good mark.','
Перевод: Я получил хорошую оценку.'),
('conversation','grade','Учёба и развитие
Пример: What grade did you get?','
Перевод: Какую оценку ты получил?'),
('conversation','homework','Учёба и развитие
Пример: I need to finish my homework.','
Перевод: Мне нужно закончить домашнее задание.'),
('conversation','assignment','Учёба и развитие
Пример: When is the assignment due?','
Перевод: Когда срок сдачи задания?'),
('conversation','project','Учёба и развитие
Пример: We are working on a project.','
Перевод: Мы работаем над проектом.'),
('conversation','research','Учёба и развитие
Пример: I need to do some research.','
Перевод: Мне нужно провести исследование.'),
('conversation','topic','Учёба и развитие
Пример: Let''s choose a topic.','
Перевод: Давайте выберем тему.'),
('conversation','chapter','Учёба и развитие
Пример: Read the first chapter.','
Перевод: Прочитайте первую главу.'),
('conversation','paragraph','Учёба и развитие
Пример: Write a short paragraph.','
Перевод: Напишите короткий абзац.'),
('conversation','note','Учёба и развитие
Пример: Let me make a note.','
Перевод: Позвольте сделать заметку.'),
('conversation','notebook','Учёба и развитие
Пример: Write it in your notebook.','
Перевод: Запишите это в тетрадь.'),
('conversation','pencil','Учёба и развитие
Пример: Use a pencil for this.','
Перевод: Для этого используйте карандаш.'),
('conversation','paper','Учёба и развитие
Пример: I need a sheet of paper.','
Перевод: Мне нужен лист бумаги.'),
('conversation','ruler','Учёба и развитие
Пример: Can I borrow your ruler?','
Перевод: Можно я возьму у вас линейку?'),
('conversation','erase','Учёба и развитие
Пример: Please erase that line.','
Перевод: Пожалуйста, сотрите эту линию.'),
('conversation','dictionary','Учёба и развитие
Пример: Look it up in a dictionary.','
Перевод: Посмотрите это в словаре.'),
('conversation','explanation','Учёба и развитие
Пример: Thank you for the explanation.','
Перевод: Спасибо за объяснение.'),
('conversation','instruction','Учёба и развитие
Пример: Read the instructions first.','
Перевод: Сначала прочитайте инструкции.'),
('conversation','method','Учёба и развитие
Пример: This method works for me.','
Перевод: Этот метод мне подходит.'),
('conversation','technique','Учёба и развитие
Пример: I am learning a new technique.','
Перевод: Я изучаю новую технику.'),
('conversation','memory','Учёба и развитие
Пример: I have a good memory for faces.','
Перевод: У меня хорошая память на лица.'),
('conversation','attention','Учёба и развитие
Пример: Please pay attention.','
Перевод: Пожалуйста, обратите внимание.'),
('conversation','focus','Учёба и развитие
Пример: I need to focus on this.','
Перевод: Мне нужно сосредоточиться на этом.'),
('conversation','list','Планирование и порядок
Пример: Make a shopping list.','
Перевод: Составьте список покупок.'),
('conversation','step','Планирование и порядок
Пример: Let''s do it step by step.','
Перевод: Давайте делать это шаг за шагом.'),
('conversation','stage','Планирование и порядок
Пример: What is the next stage?','
Перевод: Что будет следующим этапом?'),
('conversation','level','Планирование и порядок
Пример: This course is at my level.','
Перевод: Этот курс соответствует моему уровню.'),
('conversation','part','Планирование и порядок
Пример: I did not understand the last part.','
Перевод: Я не понял последнюю часть.'),
('conversation','piece','Планирование и порядок
Пример: Can I have a piece of bread?','
Перевод: Можно мне кусок хлеба?'),
('conversation','section','Планирование и порядок
Пример: Fill in this section.','
Перевод: Заполните этот раздел.'),
('conversation','group','Планирование и порядок
Пример: We work in small groups.','
Перевод: Мы работаем в небольших группах.'),
('conversation','process','Планирование и порядок
Пример: How long does the process take?','
Перевод: Сколько времени занимает процесс?'),
('conversation','system','Планирование и порядок
Пример: How does this system work?','
Перевод: Как работает эта система?'),
('conversation','structure','Планирование и порядок
Пример: The course has a clear structure.','
Перевод: Курс имеет чёткую структуру.'),
('conversation','pattern','Планирование и порядок
Пример: I noticed a pattern.','
Перевод: Я заметил закономерность.'),
('conversation','priority','Планирование и порядок
Пример: What is the main priority?','
Перевод: Каков главный приоритет?'),
('conversation','target','Планирование и порядок
Пример: We reached our target.','
Перевод: Мы достигли нашей цели.'),
('conversation','aim','Планирование и порядок
Пример: My aim is to speak clearly.','
Перевод: Моя цель — говорить ясно.'),
('conversation','action','Планирование и порядок
Пример: We need to take action.','
Перевод: Нужно принимать меры.'),
('conversation','activity','Планирование и порядок
Пример: What activities do you enjoy?','
Перевод: Какие занятия вам нравятся?'),
('conversation','arrangement','Планирование и порядок
Пример: We have an arrangement.','
Перевод: У нас есть договорённость.'),
('conversation','preparation','Планирование и порядок
Пример: The exam needs careful preparation.','
Перевод: Экзамен требует тщательной подготовки.'),
('conversation','reminder','Планирование и порядок
Пример: Set a reminder for tomorrow.','
Перевод: Установите напоминание на завтра.'),
('conversation','availability','Планирование и порядок
Пример: Can you check availability?','
Перевод: Можете проверить наличие?'),
('conversation','update','Планирование и порядок
Пример: Please give me an update.','
Перевод: Пожалуйста, дайте мне обновление.'),
('conversation','status','Планирование и порядок
Пример: What is the status of my order?','
Перевод: Каков статус моего заказа?'),
('conversation','advance','Планирование и порядок
Пример: Do I need to book in advance?','
Перевод: Нужно ли бронировать заранее?'),
('conversation','length','Размеры, формы, цвета
Пример: What is the length of the table?','
Перевод: Какова длина стола?'),
('conversation','width','Размеры, формы, цвета
Пример: Measure the width of the door.','
Перевод: Измерьте ширину двери.'),
('conversation','height','Размеры, формы, цвета
Пример: What is your height?','
Перевод: Какой у вас рост?'),
('conversation','depth','Размеры, формы, цвета
Пример: What is the depth of the pool?','
Перевод: Какова глубина бассейна?'),
('conversation','shape','Размеры, формы, цвета
Пример: What shape is it?','
Перевод: Какой у него форма?'),
('conversation','round','Размеры, формы, цвета
Пример: We need a round table.','
Перевод: Нужен круглый стол.'),
('conversation','square','Размеры, формы, цвета
Пример: Meet me in the main square.','
Перевод: Встретимся на главной площади.'),
('conversation','circle','Размеры, формы, цвета
Пример: Draw a circle.','
Перевод: Нарисуйте круг.'),
('conversation','triangle','Размеры, формы, цвета
Пример: The sign is a triangle.','
Перевод: Знак — треугольник.'),
('conversation','edge','Размеры, формы, цвета
Пример: Stay away from the edge.','
Перевод: Держитесь подальше от края.'),
('conversation','middle','Размеры, формы, цвета
Пример: Sit in the middle.','
Перевод: Сядьте посередине.'),
('conversation','top','Размеры, формы, цвета
Пример: Put your name at the top.','
Перевод: Напишите своё имя вверху.'),
('conversation','bottom','Размеры, формы, цвета
Пример: The price is at the bottom.','
Перевод: Цена указана внизу.'),
('conversation','surface','Размеры, формы, цвета
Пример: Clean the surface first.','
Перевод: Сначала очистите поверхность.'),
('conversation','angle','Размеры, формы, цвета
Пример: Look at it from another angle.','
Перевод: Посмотрите на это под другим углом.'),
('conversation','red','Размеры, формы, цвета
Пример: I like the red one.','
Перевод: Мне нравится красный.'),
('conversation','blue','Размеры, формы, цвета
Пример: The sky is blue.','
Перевод: Небо синее.'),
('conversation','green','Размеры, формы, цвета
Пример: Press the green button.','
Перевод: Нажмите зелёную кнопку.'),
('conversation','yellow','Размеры, формы, цвета
Пример: The yellow bag is mine.','
Перевод: Желёвая сумка моя.'),
('conversation','black','Размеры, формы, цвета
Пример: Black coffee, please.','
Перевод: Чёрный кофе, пожалуйста.'),
('conversation','white','Размеры, формы, цвета
Пример: I need a white shirt.','
Перевод: Мне нужна белая рубашка.'),
('conversation','brown','Размеры, формы, цвета
Пример: The brown shoes are cheaper.','
Перевод: Коричневые туфли дешевле.'),
('conversation','gray','Размеры, формы, цвета
Пример: The sky is gray today.','
Перевод: Сегодня небо серое.'),
('conversation','pink','Размеры, формы, цвета
Пример: She chose a pink dress.','
Перевод: Она выбрала розовое платье.'),
('conversation','purple','Размеры, формы, цвета
Пример: I like purple flowers.','
Перевод: Мне нравятся фиолетовые цветы.'),
('conversation','golden','Размеры, формы, цвета
Пример: The bread is golden brown.','
Перевод: Хлеб золотисто-коричневый.'),
('conversation','silver','Размеры, формы, цвета
Пример: It is a silver ring.','
Перевод: Это серебряное кольцо.'),
('conversation','bright','Размеры, формы, цвета
Пример: The light is too bright.','
Перевод: Свет слишком яркий.'),
('conversation','dark','Размеры, формы, цвета
Пример: It gets dark early.','
Перевод: Темнеет рано.'),
('conversation','wood','Материалы и полезные предметы
Пример: The table is made of wood.','
Перевод: Стол сделан из дерева.'),
('conversation','wooden','Материалы и полезные предметы
Пример: I need a wooden spoon.','
Перевод: Мне нужна деревянная ложка.'),
('conversation','metal','Материалы и полезные предметы
Пример: This box is made of metal.','
Перевод: Эта коробка из металла.'),
('conversation','steel','Материалы и полезные предметы
Пример: The sink is made of steel.','
Перевод: Раковина из стали.'),
('conversation','plastic','Материалы и полезные предметы
Пример: Do you need a plastic bag?','
Перевод: Вам нужен пластиковый пакет?'),
('conversation','rubber','Материалы и полезные предметы
Пример: These boots are made of rubber.','
Перевод: Эти ботинки из резины.'),
('conversation','leather','Материалы и полезные предметы
Пример: These shoes are made of leather.','
Перевод: Эти туфли из кожи.'),
('conversation','cotton','Материалы и полезные предметы
Пример: I prefer cotton shirts.','
Перевод: Я предпочитаю хлопковые рубашки.'),
('conversation','wool','Материалы и полезные предметы
Пример: This sweater is made of wool.','
Перевод: Этот свитер из шерсти.'),
('conversation','fabric','Материалы и полезные предметы
Пример: This fabric is easy to wash.','
Перевод: Эта ткань легко стирается.'),
('conversation','material','Материалы и полезные предметы
Пример: What material is this?','
Перевод: Из какого это материала?'),
('conversation','stone','Материалы и полезные предметы
Пример: There is a stone in my shoe.','
Перевод: В моей обуви камень.'),
('conversation','brick','Материалы и полезные предметы
Пример: The house is made of brick.','
Перевод: Дом построен из кирпича.'),
('conversation','concrete','Материалы и полезные предметы
Пример: The floor is made of concrete.','
Перевод: Пол из бетона.'),
('conversation','rope','Материалы и полезные предметы
Пример: We need a strong rope.','
Перевод: Нужна прочная верёвка.'),
('conversation','string','Материалы и полезные предметы
Пример: Tie it with string.','
Перевод: Привяжи это ниткой.'),
('conversation','wire','Материалы и полезные предметы
Пример: Do not touch the wire.','
Перевод: Не трогай провод.'),
('conversation','tape','Материалы и полезные предметы
Пример: Do you have some tape?','
Перевод: У вас есть скотч?'),
('conversation','glue','Материалы и полезные предметы
Пример: Use a little glue.','
Перевод: Возьми немного клея.'),
('conversation','scissors','Материалы и полезные предметы
Пример: Can I borrow your scissors?','
Перевод: Можно одолжить твои ножницы?'),
('conversation','knife','Материалы и полезные предметы
Пример: Be careful with the knife.','
Перевод: Будь осторожен с ножом.'),
('conversation','fork','Материалы и полезные предметы
Пример: I need a knife and fork.','
Перевод: Мне нужны нож и вилка.'),
('conversation','pan','Материалы и полезные предметы
Пример: Heat the oil in a pan.','
Перевод: Разогрейте масло в сковороде.'),
('conversation','pot','Материалы и полезные предметы
Пример: Put the rice in a pot.','
Перевод: Положите рис в кастрюлю.'),
('conversation','bowl','Материалы и полезные предметы
Пример: A bowl of soup, please.','
Перевод: Чашка супа, пожалуйста.'),
('conversation','tray','Материалы и полезные предметы
Пример: Put the cups on the tray.','
Перевод: Положите чашки на поднос.'),
('conversation','lid','Материалы и полезные предметы
Пример: Put the lid on.','
Перевод: Накройте крышкой.'),
('conversation','bucket','Материалы и полезные предметы
Пример: Fill the bucket with water.','
Перевод: Заполните ведро водой.'),
('conversation','basket','Материалы и полезные предметы
Пример: Put the fruit in the basket.','
Перевод: Положите фрукты в корзину.'),
('conversation','animal','Животные и окружающий мир
Пример: Do you like animals?','
Перевод: Тебе нравятся животные?'),
('conversation','pet','Животные и окружающий мир
Пример: Do you have any pets?','
Перевод: У тебя есть домашние животные?'),
('conversation','dog','Животные и окружающий мир
Пример: Can I bring my dog?','
Перевод: Можно привести мою собаку?'),
('conversation','cat','Животные и окружающий мир
Пример: The cat is sleeping.','
Перевод: Кот спит.'),
('conversation','bird','Животные и окружающий мир
Пример: I can hear birds outside.','
Перевод: Я слышу птиц снаружи.'),
('conversation','horse','Животные и окружающий мир
Пример: Can you ride a horse?','
Перевод: Ты умеешь ездить верхом?'),
('conversation','cow','Животные и окружающий мир
Пример: There are cows in the field.','
Перевод: На поле пасутся коровы.'),
('conversation','sheep','Животные и окружающий мир
Пример: There are sheep on the farm.','
Перевод: На ферме пасутся овцы.'),
('conversation','pig','Животные и окружающий мир
Пример: The farm has pigs.','
Перевод: На ферме есть свиньи.'),
('conversation','duck','Животные и окружающий мир
Пример: There are ducks on the lake.','
Перевод: На озере плавают утки.'),
('conversation','rabbit','Животные и окружающий мир
Пример: The children have a pet rabbit.','
Перевод: У детей есть домашний кролик.'),
('conversation','insect','Животные и окружающий мир
Пример: An insect bit me.','
Перевод: Меня укусил насекомое.'),
('conversation','bee','Животные и окружающий мир
Пример: A bee stung my hand.','
Перевод: Меня ужалила пчела в руку.'),
('conversation','mosquito','Животные и окружающий мир
Пример: There are mosquitoes outside.','
Перевод: Снаружи много комаров.'),
('conversation','fly','Животные и окружающий мир
Пример: There is a fly in the kitchen.','
Перевод: На кухне летит муха.'),
('conversation','spider','Животные и окружающий мир
Пример: There is a spider on the wall.','
Перевод: На стене ползёт паук.'),
('conversation','bite','Животные и окружающий мир
Пример: Does your dog bite?','
Перевод: Ваши собака кусается?'),
('conversation','feed','Животные и окружающий мир
Пример: Please feed the cat.','
Перевод: Пожалуйста, покормите кота.'),
('conversation','wild','Животные и окружающий мир
Пример: Do not feed wild animals.','
Перевод: Не кормите диких животных.'),
('conversation','farm','Животные и окружающий мир
Пример: My uncle works on a farm.','
Перевод: Мой дядя работает на ферме.'),
('conversation','field','Животные и окружающий мир
Пример: The children played in the field.','
Перевод: Дети играли на поле.'),
('conversation','hill','Животные и окружающий мир
Пример: The house is on a hill.','
Перевод: Дом находится на холме.'),
('conversation','valley','Животные и окружающий мир
Пример: We drove through the valley.','
Перевод: Мы проехали через долину.'),
('conversation','island','Животные и окружающий мир
Пример: We stayed on a small island.','
Перевод: Мы остановились на небольшом острове.'),
('conversation','coast','Животные и окружающий мир
Пример: They live on the coast.','
Перевод: Они живут на побережье.'),
('conversation','ocean','Животные и окружающий мир
Пример: I have never seen the ocean.','
Перевод: Я никогда не видел океан.'),
('conversation','land','Животные и окружающий мир
Пример: The plane will land soon.','
Перевод: Самолёт скоро приземлится.'),
('conversation','ground','Животные и окружающий мир
Пример: Put the bag on the ground.','
Перевод: Положите сумку на землю.'),
('conversation','environment','Животные и окружающий мир
Пример: We should protect the environment.','
Перевод: Мы должны защищать окружающую среду.'),
('conversation','laundry','Чистота и забота о доме
Пример: I need to do the laundry.','
Перевод: Мне нужно постирать бельё.'),
('conversation','detergent','Чистота и забота о доме
Пример: Where is the laundry detergent?','
Перевод: Где моющее средство для стирки?'),
('conversation','dishwasher','Чистота и забота о доме
Пример: Is the dishwasher working?','
Перевод: Работает ли посудомоечная машина?'),
('conversation','vacuum','Чистота и забота о доме
Пример: I need to vacuum the carpet.','
Перевод: Мне нужно пропылесосить ковер.'),
('conversation','mop','Чистота и забота о доме
Пример: Please mop the floor.','
Перевод: Пожалуйста, помойте пол.'),
('conversation','broom','Чистота и забота о доме
Пример: Use the broom to sweep the floor.','
Перевод: Используйте метлу, чтобы подмести пол.'),
('conversation','dust','Чистота и забота о доме
Пример: There is dust on the shelf.','
Перевод: На полке пыль.'),
('conversation','dirty','Чистота и забота о доме
Пример: My clothes are dirty.','
Перевод: Моя одежда грязная.'),
('conversation','messy','Чистота и забота о доме
Пример: My room is messy.','
Перевод: В моей комнате беспорядок.'),
('conversation','tidy','Чистота и забота о доме
Пример: Keep your room tidy.','
Перевод: Держите свою комнату в порядке.'),
('conversation','stain','Чистота и забота о доме
Пример: There is a stain on my shirt.','
Перевод: На моей рубашке пятно.'),
('conversation','spill','Чистота и забота о доме
Пример: I spilled coffee on the table.','
Перевод: Я пролил кофе на стол.'),
('conversation','rinse','Чистота и забота о доме
Пример: Rinse the cup with water.','
Перевод: Промойте чашку водой.'),
('conversation','soak','Чистота и забота о доме
Пример: Soak the shirt before washing.','
Перевод: Замочите рубашку перед стиркой.'),
('conversation','drain','Чистота и забота о доме
Пример: The drain is blocked.','
Перевод: Слив заблокирован.'),
('conversation','blocked','Чистота и забота о доме
Пример: The sink is blocked.','
Перевод: Раковина заблокирована.'),
('conversation','bin','Чистота и забота о доме
Пример: Put it in the bin.','
Перевод: Положите это в мусорное ведро.'),
('conversation','recycle','Чистота и забота о доме
Пример: Can we recycle this bottle?','
Перевод: Можно ли переработать эту бутылку?'),
('conversation','waste','Чистота и забота о доме
Пример: Do not waste water.','
Перевод: Не тратьте воду.'),
('conversation','reusable','Чистота и забота о доме
Пример: I carry a reusable bag.','
Перевод: Я ношу многоразовую сумку.'),
('conversation','disposable','Чистота и забота о доме
Пример: Do you have disposable cups?','
Перевод: У вас есть одноразовые стаканчики?'),
('conversation','storage','Чистота и забота о доме
Пример: Is there space for storage?','
Перевод: Есть место для хранения?'),
('conversation','space','Чистота и забота о доме
Пример: We need more space.','
Перевод: Нужнее пространство.'),
('conversation','mess','Чистота и забота о доме
Пример: Sorry about the mess.','
Перевод: Извините за беспорядок.'),
('conversation','pile','Чистота и забота о доме
Пример: There is a pile of clothes on the bed.','
Перевод: На кровати лежит куча одежды.'),
('conversation','maintain','Чистота и забота о доме
Пример: It is expensive to maintain this car.','
Перевод: Эксплуатация этой машины дорогая.'),
('conversation','maintenance','Чистота и забота о доме
Пример: Who pays for maintenance?','
Перевод: Кто платит за обслуживание?'),
('conversation','bulb','Чистота и забота о доме
Пример: The light bulb needs replacing.','
Перевод: Лампочку нужно заменить.'),
('conversation','enjoy','Чуть точнее о действиях
Пример: I enjoy learning languages.','
Перевод: Мне нравится изучать языки.'),
('conversation','hate','Чуть точнее о действиях
Пример: I hate waiting in traffic.','
Перевод: Я ненавижу стоять в пробке.'),
('conversation','regret','Чуть точнее о действиях
Пример: I regret what I said.','
Перевод: Я жалею о сказанном.'),
('conversation','forgive','Чуть точнее о действиях
Пример: Please forgive me.','
Перевод: Пожалуйста, простите меня.'),
('conversation','blame','Чуть точнее о действиях
Пример: Do not blame yourself.','
Перевод: Не вините себя.'),
('conversation','complain','Чуть точнее о действиях
Пример: I want to complain about the service.','
Перевод: Я хочу пожаловаться на обслуживание.'),
('conversation','argue','Чуть точнее о действиях
Пример: I do not want to argue.','
Перевод: Я не хочу спорить.'),
('conversation','interrupt','Чуть точнее о действиях
Пример: Sorry to interrupt.','
Перевод: Извините за прерывание.'),
('conversation','bother','Чуть точнее о действиях
Пример: Sorry to bother you.','
Перевод: Извините, что беспокою вас.'),
('conversation','annoy','Чуть точнее о действиях
Пример: That noise annoys me.','
Перевод: Этот шум меня раздражает.'),
('conversation','disturb','Чуть точнее о действиях
Пример: Please do not disturb me.','
Перевод: Пожалуйста, не беспокойте меня.'),
('conversation','encourage','Чуть точнее о действиях
Пример: My teacher encourages me to speak.','
Перевод: Мой учитель поощряет меня говорить.'),
('conversation','remind','Чуть точнее о действиях
Пример: Please remind me tomorrow.','
Перевод: Пожалуйста, напомните мне завтра.'),
('conversation','warn','Чуть точнее о действиях
Пример: They warned us about the traffic.','
Перевод: Они предупредили нас о пробках.'),
('conversation','introduce','Чуть точнее о действиях
Пример: Let me introduce my friend.','
Перевод: Позвольте представить моего друга.'),
('conversation','greet','Чуть точнее о действиях
Пример: She greeted us with a smile.','
Перевод: Она поприветствовала нас с улыбкой.'),
('conversation','celebrate','Чуть точнее о действиях
Пример: Let''s celebrate your birthday.','
Перевод: Давайте отпразднуем ваш день рождения.'),
('conversation','behave','Чуть точнее о действиях
Пример: The children behaved well.','
Перевод: Дети вели себя хорошо.'),
('conversation','treat','Чуть точнее о действиях
Пример: Please treat people with respect.','
Перевод: Пожалуйста, относитесь к людям с уважением.'),
('conversation','attend','Чуть точнее о действиях
Пример: Can you attend the meeting?','
Перевод: Вы сможете присутствовать на встрече?'),
('conversation','participate','Чуть точнее о действиях
Пример: Everyone can participate.','
Перевод: Каждый может участвовать.'),
('conversation','above','Место, путь и положение
Пример: The flat above us is empty.','
Перевод: Квартира над нами пустует.'),
('conversation','below','Место, путь и положение
Пример: The temperature is below zero.','
Перевод: Температура ниже нуля.'),
('conversation','across','Место, путь и положение
Пример: The bank is across the street.','
Перевод: Банк находится через улицу.'),
('conversation','along','Место, путь и положение
Пример: Walk along this road.','
Перевод: Идите вдоль этой дороги.'),
('conversation','around','Место, путь и положение
Пример: Let''s walk around the park.','
Перевод: Прогуляемся вокруг парка.'),
('conversation','through','Место, путь и положение
Пример: Go through that door.','
Перевод: Пройдите через эту дверь.'),
('conversation','toward','Место, путь и положение
Пример: Walk toward the station.','
Перевод: Идите к станции.'),
('conversation','beyond','Место, путь и положение
Пример: The village is beyond the bridge.','
Перевод: Деревня находится за мостом.'),
('conversation','opposite','Место, путь и положение
Пример: The pharmacy is opposite the bank.','
Перевод: Аптека находится напротив банка.'),
('conversation','beside','Место, путь и положение
Пример: Sit beside me.','
Перевод: Сядьте рядом со мной.'),
('conversation','among','Место, путь и положение
Пример: I found it among my papers.','
Перевод: Я нашёл это среди своих бумаг.'),
('conversation','onto','Место, путь и положение
Пример: Put the bag onto the shelf.','
Перевод: Положите сумку на полку.'),
('conversation','into','Место, путь и положение
Пример: Come into the kitchen.','
Перевод: Зайдите на кухню.'),
('conversation','within','Место, путь и положение
Пример: We will reply within two days.','
Перевод: Мы ответим в течение двух дней.'),
('conversation','forward','Место, путь и положение
Пример: Move forward a little.','
Перевод: Продвиньтесь немного вперёд.'),
('conversation','backward','Место, путь и положение
Пример: Take one step backward.','
Перевод: Сделайте шаг назад.'),
('conversation','ahead','Место, путь и положение
Пример: The hotel is just ahead.','
Перевод: Отель находится прямо впереди.'),
('conversation','nearby','Место, путь и положение
Пример: Is there a pharmacy nearby?','
Перевод: Есть ли рядом аптека?'),
('conversation','apart','Место, путь и положение
Пример: We live far apart.','
Перевод: Мы живём далеко друг от друга.'),
('conversation','aside','Место, путь и положение
Пример: Please step aside.','
Перевод: Пожалуйста, отойдите в сторону.'),
('conversation','outdoors','Место, путь и положение
Пример: I like working outdoors.','
Перевод: Мне нравится работать на открытом воздухе.'),
('conversation','indoors','Место, путь и положение
Пример: Let''s stay indoors today.','
Перевод: Давайте сегодня останемся в помещении.'),
('conversation','neat','Точнее описывать людей и вещи
Пример: Keep your notes neat.','
Перевод: Держите свои записи аккуратными.'),
('conversation','gentle','Точнее описывать людей и вещи
Пример: Be gentle with the baby.','
Перевод: Будьте нежны с ребёнком.'),
('conversation','rough','Точнее описывать людей и вещи
Пример: The road is rough.','
Перевод: Дорога неровная.'),
('conversation','smooth','Точнее описывать людей и вещи
Пример: The surface is smooth.','
Перевод: Поверхность гладкая.'),
('conversation','sharp','Точнее описывать людей и вещи
Пример: Be careful, the knife is sharp.','
Перевод: Будьте осторожны, нож острый.'),
('conversation','blunt','Точнее описывать людей и вещи
Пример: This knife is blunt.','
Перевод: Этот нож тупой.'),
('conversation','loose','Точнее описывать людей и вещи
Пример: This screw is loose.','
Перевод: Этот винт ослаблен.'),
('conversation','tight','Точнее описывать людей и вещи
Пример: These shoes are too tight.','
Перевод: Эти туфли слишком тесные.'),
('conversation','firm','Точнее описывать людей и вещи
Пример: I prefer a firm mattress.','
Перевод: Я предпочитаю жёсткий матрас.'),
('conversation','solid','Точнее описывать людей и вещи
Пример: This table is solid wood.','
Перевод: Этот стол из массива дерева.'),
('conversation','liquid','Точнее описывать людей и вещи
Пример: Can I take liquids on the plane?','
Перевод: Можно ли брать жидкости в самолёт?'),
('conversation','plain','Точнее описывать людей и вещи
Пример: I would like plain yogurt.','
Перевод: Я бы хотел простой йогурт.'),
('conversation','fancy','Точнее описывать людей и вещи
Пример: We do not need a fancy restaurant.','
Перевод: Нам не нужен дорогой ресторан.'),
('conversation','casual','Точнее описывать людей и вещи
Пример: Wear casual clothes.','
Перевод: Носите повседневную одежду.'),
('conversation','formal','Точнее описывать людей и вещи
Пример: Is it a formal meeting?','
Перевод: Это официальная встреча?'),
('conversation','exact','Точнее описывать людей и вещи
Пример: What is the exact address?','
Перевод: Какой точный адрес?'),
('conversation','approximate','Точнее описывать людей и вещи
Пример: What is the approximate cost?','
Перевод: Какова приблизительная стоимость?'),
('conversation','accurate','Точнее описывать людей и вещи
Пример: Is this information accurate?','
Перевод: Эта информация точна?'),
('conversation','certain','Точнее описывать людей и вещи
Пример: Are you certain about that?','
Перевод: Вы уверены в этом?'),
('conversation','likely','Точнее описывать людей и вещи
Пример: It is likely to rain.','
Перевод: Вероятно, будет дождь.'),
('conversation','unlikely','Точнее описывать людей и вещи
Пример: That is unlikely to happen.','
Перевод: Это маловероятно.'),
('conversation','aware','Точнее описывать людей и вещи
Пример: I was not aware of the change.','
Перевод: Я не знал об изменении.'),
('conversation','familiar','Точнее описывать людей и вещи
Пример: This place looks familiar.','
Перевод: Это место выглядит знакомо.'),
('conversation','unfamiliar','Точнее описывать людей и вещи
Пример: I am unfamiliar with this area.','
Перевод: Я не знаком с этим районом.'),
('conversation','personal','Точнее описывать людей и вещи
Пример: Can I ask a personal question?','
Перевод: Можно задать личный вопрос?'),
('conversation','independent','Точнее описывать людей и вещи
Пример: I want to be independent.','
Перевод: Я хочу быть независимым.'),
('conversation','dependent','Точнее описывать людей и вещи
Пример: The price is dependent on the size.','
Перевод: Цена зависит от размера.'),
('conversation','business','Рабочий разговор без сложных слов
Пример: I am here on business.','
Перевод: Я здесь по делам.'),
('conversation','client','Рабочий разговор без сложных слов
Пример: I have a meeting with a client.','
Перевод: У меня запланирована встреча с клиентом.'),
('conversation','staff','Рабочий разговор без сложных слов
Пример: The staff are friendly.','
Перевод: Персонал дружелюбный.'),
('conversation','department','Рабочий разговор без сложных слов
Пример: Which department do you work in?','
Перевод: В каком отделе вы работаете?'),
('conversation','assistant','Рабочий разговор без сложных слов
Пример: Ask my assistant.','
Перевод: Спросите моего помощника.'),
('conversation','director','Рабочий разговор без сложных слов
Пример: The director is in a meeting.','
Перевод: Директор находится на встрече.'),
('conversation','supervisor','Рабочий разговор без сложных слов
Пример: Talk to your supervisor.','
Перевод: Обратитесь к своему руководителю.'),
('conversation','workshop','Рабочий разговор без сложных слов
Пример: I attended a workshop.','
Перевод: Я посетил(а) семинар.'),
('conversation','presentation','Рабочий разговор без сложных слов
Пример: I am preparing a presentation.','
Перевод: Я готовлю презентацию.'),
('conversation','discussion','Рабочий разговор без сложных слов
Пример: We had a useful discussion.','
Перевод: У нас была полезная дискуссия.'),
('conversation','feedback','Рабочий разговор без сложных слов
Пример: Can you give me some feedback?','
Перевод: Можете дать мне обратную связь?'),
('conversation','suggestion','Рабочий разговор без сложных слов
Пример: I have a suggestion.','
Перевод: У меня есть предложение.'),
('conversation','proposal','Рабочий разговор без сложных слов
Пример: Thank you for your proposal.','
Перевод: Спасибо за ваше предложение.'),
('conversation','agreement','Рабочий разговор без сложных слов
Пример: We reached an agreement.','
Перевод: Мы пришли к соглашению.'),
('conversation','requirement','Рабочий разговор без сложных слов
Пример: What are the main requirements?','
Перевод: Каковы основные требования?'),
('conversation','responsibility','Рабочий разговор без сложных слов
Пример: What are my responsibilities?','
Перевод: Каковы мои обязанности?'),
('conversation','duty','Рабочий разговор без сложных слов
Пример: Who is on duty today?','
Перевод: Кто сегодня дежурит?'),
('conversation','procedure','Рабочий разговор без сложных слов
Пример: What is the correct procedure?','
Перевод: Какова правильная процедура?'),
('conversation','policy','Рабочий разговор без сложных слов
Пример: What is your return policy?','
Перевод: Какова ваша политика возврата?'),
('conversation','safety','Рабочий разговор без сложных слов
Пример: Safety comes first.','
Перевод: Безопасность превыше всего.'),
('conversation','quality','Рабочий разговор без сложных слов
Пример: The quality is good.','
Перевод: Качество хорошее.'),
('conversation','quantity','Рабочий разговор без сложных слов
Пример: What quantity do you need?','
Перевод: Какое количество вам нужно?'),
('conversation','product','Рабочий разговор без сложных слов
Пример: Is this product available?','
Перевод: Этот товар в наличии?'),
('conversation','sample','Рабочий разговор без сложных слов
Пример: Can you send me a sample?','
Перевод: Можете прислать образец?'),
('conversation','stock','Рабочий разговор без сложных слов
Пример: Is it in stock?','
Перевод: Есть ли он в наличии?'),
('conversation','supplier','Рабочий разговор без сложных слов
Пример: We need a new supplier.','
Перевод: Нам нужен новый поставщик.'),
('conversation','government','Общественная жизнь и новости
Пример: The government announced new rules.','
Перевод: Правительство объявило новые правила.'),
('conversation','president','Общественная жизнь и новости
Пример: Who is the president?','
Перевод: Кто президент?'),
('conversation','election','Общественная жизнь и новости
Пример: When is the next election?','
Перевод: Когда следующие выборы?'),
('conversation','vote','Общественная жизнь и новости
Пример: Where can I vote?','
Перевод: Где я могу проголосовать?'),
('conversation','population','Общественная жизнь и новости
Пример: The population is growing.','
Перевод: Население растёт.'),
('conversation','society','Общественная жизнь и новости
Пример: Education helps society.','
Перевод: Образование помогает обществу.'),
('conversation','economy','Общественная жизнь и новости
Пример: The economy is changing.','
Перевод: Экономика меняется.'),
('conversation','economic','Общественная жизнь и новости
Пример: We discussed the economic situation.','
Перевод: Мы обсудили экономическую ситуацию.'),
('conversation','political','Общественная жизнь и новости
Пример: I avoid political arguments at work.','
Перевод: Я избегаю политических споров на работе.'),
('conversation','social','Общественная жизнь и новости
Пример: I enjoy social activities.','
Перевод: Мне нравятся общественные мероприятия.'),
('conversation','national','Общественная жизнь и новости
Пример: Tomorrow is a national holiday.','
Перевод: Завтра национальный праздник.'),
('conversation','international','Общественная жизнь и новости
Пример: This is an international company.','
Перевод: Это международная компания.'),
('conversation','global','Общественная жизнь и новости
Пример: It is a global problem.','
Перевод: Это глобальная проблема.'),
('conversation','human','Общественная жизнь и новости
Пример: Everyone makes human errors.','
Перевод: Все совершают человеческие ошибки.'),
('conversation','rights','Общественная жизнь и новости
Пример: I want to understand my rights.','
Перевод: Я хочу понять свои права.'),
('conversation','peaceful','Общественная жизнь и новости
Пример: It is a peaceful neighborhood.','
Перевод: Это тихий район.'),
('conversation','violent','Общественная жизнь и новости
Пример: I do not like violent films.','
Перевод: Мне не нравятся насильственные фильмы.'),
('conversation','crime','Общественная жизнь и новости
Пример: Crime is a concern here.','
Перевод: Преступность здесь вызывает беспокойство.'),
('conversation','prison','Общественная жизнь и новости
Пример: He spent a year in prison.','
Перевод: Он провёл год в тюрьме.'),
('conversation','court','Общественная жизнь и новости
Пример: The case went to court.','
Перевод: Дело дошло до суда.'),
('conversation','judge','Общественная жизнь и новости
Пример: The judge made a decision.','
Перевод: Судья вынес решение.'),
('conversation','protest','Общественная жизнь и новости
Пример: There was a peaceful protest.','
Перевод: Был мирный протест.'),
('conversation','awake','Сон, здоровье и личные привычки
Пример: Are you still awake?','
Перевод: Ты всё ещё не спишь?'),
('conversation','asleep','Сон, здоровье и личные привычки
Пример: The baby is asleep.','
Перевод: Малыш спит.'),
('conversation','sleepy','Сон, здоровье и личные привычки
Пример: I feel sleepy.','
Перевод: Я чувствую сонливость.'),
('conversation','dream','Сон, здоровье и личные привычки
Пример: I had a strange dream.','
Перевод: Мне приснился странный сон.'),
('conversation','nightmare','Сон, здоровье и личные привычки
Пример: I had a nightmare last night.','
Перевод: Вчера ночью у меня был кошмар.'),
('conversation','snore','Сон, здоровье и личные привычки
Пример: Do I snore?','
Перевод: Я храплю?'),
('conversation','alarm','Сон, здоровье и личные привычки
Пример: Set the alarm for seven.','
Перевод: Установи будильник на семь.'),
('conversation','nap','Сон, здоровье и личные привычки
Пример: I need a short nap.','
Перевод: Мне нужен короткий сон.'),
('conversation','tiredness','Сон, здоровье и личные привычки
Пример: I have been feeling a lot of tiredness.','
Перевод: Я чувствую сильную усталость.'),
('conversation','weakness','Сон, здоровье и личные привычки
Пример: I feel weakness in my legs.','
Перевод: Я ощущаю слабость в ногах.'),
('conversation','fitness','Сон, здоровье и личные привычки
Пример: I want to improve my fitness.','
Перевод: Я хочу улучшить свою физическую форму.'),
('conversation','diet','Сон, здоровье и личные привычки
Пример: I am trying to improve my diet.','
Перевод: Я пытаюсь улучшить свой рацион.'),
('conversation','appetite','Сон, здоровье и личные привычки
Пример: I have lost my appetite.','
Перевод: У меня пропал аппетит.'),
('conversation','vegetarian','Сон, здоровье и личные привычки
Пример: Do you have vegetarian food?','
Перевод: У вас есть вегетарианская еда?'),
('conversation','vegan','Сон, здоровье и личные привычки
Пример: Is this dish vegan?','
Перевод: Это блюдо веганское?'),
('conversation','balanced','Сон, здоровье и личные привычки
Пример: I try to eat a balanced diet.','
Перевод: Я стараюсь питаться сбалансировано.'),
('conversation','regular','Сон, здоровье и личные привычки
Пример: I need regular breaks.','
Перевод: Мне нужны регулярные перерывы.'),
('conversation','movement','Сон, здоровье и личные привычки
Пример: This movement hurts my shoulder.','
Перевод: Это движение болит в плече.'),
('conversation','posture','Сон, здоровье и личные привычки
Пример: My posture needs work.','
Перевод: Моя осанка требует улучшения.'),
('conversation','breath','Сон, здоровье и личные привычки
Пример: I am out of breath.','
Перевод: Мне не хватает воздуха.'),
('conversation','heartbeat','Сон, здоровье и личные привычки
Пример: My heartbeat feels fast.','
Перевод: Сердцебиение кажется быстрым.'),
('conversation','pulse','Сон, здоровье и личные привычки
Пример: The nurse checked my pulse.','
Перевод: Медсестра проверила мой пульс.'),
('conversation','chest','Сон, здоровье и личные привычки
Пример: I have pain in my chest.','
Перевод: У меня боль в груди.'),
('conversation','lung','Сон, здоровье и личные привычки
Пример: Smoking damages the lungs.','
Перевод: Курение вредит лёгким.'),
('conversation','elbow','Сон, здоровье и личные привычки
Пример: I hit my elbow.','
Перевод: Я ударил локоть.'),
('conversation','wrist','Сон, здоровье и личные привычки
Пример: My wrist hurts.','
Перевод: У меня болит запястье.'),
('conversation','ankle','Сон, здоровье и личные привычки
Пример: I twisted my ankle.','
Перевод: Я вывихнул лодыжку.'),
('conversation','toe','Сон, здоровье и личные привычки
Пример: I hurt my toe.','
Перевод: Я повредил палец ноги.'),
('conversation','signal','Онлайн-жизнь и сообщения
Пример: There is no phone signal here.','
Перевод: Здесь нет сигнала телефона.'),
('conversation','network','Онлайн-жизнь и сообщения
Пример: I cannot connect to the network.','
Перевод: Не могу подключиться к сети.'),
('conversation','wireless','Онлайн-жизнь и сообщения
Пример: Do you have wireless internet?','
Перевод: У вас есть беспроводной интернет?'),
('conversation','mobile','Онлайн-жизнь и сообщения
Пример: What is your mobile number?','
Перевод: Какой у вас мобильный номер?'),
('conversation','device','Онлайн-жизнь и сообщения
Пример: Can I connect another device?','
Перевод: Могу я подключить другое устройство?'),
('conversation','tablet','Онлайн-жизнь и сообщения
Пример: I read books on my tablet.','
Перевод: Я читаю книги на планшете.'),
('conversation','speaker','Онлайн-жизнь и сообщения
Пример: The speaker is too loud.','
Перевод: Громкость динамика слишком высока.'),
('conversation','microphone','Онлайн-жизнь и сообщения
Пример: My microphone is not working.','
Перевод: Мой микрофон не работает.'),
('conversation','headphones','Онлайн-жизнь и сообщения
Пример: Can I use my headphones?','
Перевод: Можно ли использовать наушники?'),
('conversation','volume','Онлайн-жизнь и сообщения
Пример: Turn down the volume.','
Перевод: Уменьшите громкость.'),
('conversation','setting','Онлайн-жизнь и сообщения
Пример: Check the sound settings.','
Перевод: Проверьте настройки звука.'),
('conversation','notification','Онлайн-жизнь и сообщения
Пример: I turned off notifications.','
Перевод: Я отключил уведомления.'),
('conversation','profile','Онлайн-жизнь и сообщения
Пример: Update your profile photo.','
Перевод: Обновите фото профиля.'),
('conversation','username','Онлайн-жизнь и сообщения
Пример: I forgot my username.','
Перевод: Я забыл своё имя пользователя.'),
('conversation','privacy','Онлайн-жизнь и сообщения
Пример: Check your privacy settings.','
Перевод: Проверьте настройки конфиденциальности.'),
('conversation','attachment','Онлайн-жизнь и сообщения
Пример: Please open the attachment.','
Перевод: Пожалуйста, откройте вложение.'),
('conversation','folder','Онлайн-жизнь и сообщения
Пример: Save it in this folder.','
Перевод: Сохраните его в этой папке.'),
('conversation','delete','Онлайн-жизнь и сообщения
Пример: Can I delete this message?','
Перевод: Можно ли удалить это сообщение?'),
('conversation','edit','Онлайн-жизнь и сообщения
Пример: I need to edit this document.','
Перевод: Мне нужно отредактировать этот документ.'),
('conversation','install','Онлайн-жизнь и сообщения
Пример: How do I install this app?','
Перевод: Как установить это приложение?'),
('conversation','restart','Онлайн-жизнь и сообщения
Пример: Try restarting your phone.','
Перевод: Попробуйте перезагрузить телефон.'),
('conversation','reset','Онлайн-жизнь и сообщения
Пример: I need to reset my password.','
Перевод: Мне нужно сбросить пароль.'),
('conversation','click','Онлайн-жизнь и сообщения
Пример: Click on the link.','
Перевод: Нажмите на ссылку.'),
('conversation','scroll','Онлайн-жизнь и сообщения
Пример: Scroll down to the bottom.','
Перевод: Прокрутите вниз до конца.'),
('conversation','reply','Онлайн-жизнь и сообщения
Пример: Please reply when you have time.','
Перевод: Пожалуйста, ответьте, когда будет время.'),
('conversation','attach','Онлайн-жизнь и сообщения
Пример: Please attach your receipt.','
Перевод: Пожалуйста, приложите чек.'),
('conversation','describe','Завершить разговор и выразить мысль точнее
Пример: Can you describe the problem?','
Перевод: Можете описать проблему?'),
('conversation','mention','Завершить разговор и выразить мысль точнее
Пример: You did not mention the price.','
Перевод: Вы не указали цену.'),
('conversation','express','Завершить разговор и выразить мысль точнее
Пример: It is hard to express this in English.','
Перевод: Трудно выразить это по‑английски.'),
('conversation','communicate','Завершить разговор и выразить мысль точнее
Пример: I want to communicate clearly.','
Перевод: Я хочу общаться ясно.'),
('conversation','respond','Завершить разговор и выразить мысль точнее
Пример: Please respond to my message.','
Перевод: Пожалуйста, ответьте на моё сообщение.'),
('conversation','clarify','Завершить разговор и выразить мысль точнее
Пример: Could you clarify that point?','
Перевод: Не могли бы вы уточнить этот момент?'),
('conversation','summarize','Завершить разговор и выразить мысль точнее
Пример: Let me summarize what we agreed.','
Перевод: Позвольте подвести итог нашему согласию.'),
('conversation','recognize','Завершить разговор и выразить мысль точнее
Пример: I did not recognize you.','
Перевод: Я вас не узнал.'),
('conversation','imagine','Завершить разговор и выразить мысль точнее
Пример: Imagine living near the sea.','
Перевод: Представьте, что живёте у моря.'),
('conversation','suppose','Завершить разговор и выразить мысль точнее
Пример: I suppose you are right.','
Перевод: Полагаю, вы правы.'),
('conversation','wonder','Завершить разговор и выразить мысль точнее
Пример: I wonder if the shop is open.','
Перевод: Интересно, открыт ли магазин.'),
('conversation','admit','Завершить разговор и выразить мысль точнее
Пример: I admit I made a mistake.','
Перевод: Признаю, я ошибся.'),
('conversation','deny','Завершить разговор и выразить мысль точнее
Пример: He denied taking the money.','
Перевод: Он отрекся от того, что взял деньги.'),
('conversation','insist','Завершить разговор и выразить мысль точнее
Пример: I insist on paying my share.','
Перевод: Я настаиваю на том, чтобы заплатить свою часть.'),
('conversation','convince','Завершить разговор и выразить мысль точнее
Пример: Can you convince me?','
Перевод: Можете меня убедить?'),
('conversation','persuade','Завершить разговор и выразить мысль точнее
Пример: She persuaded me to try again.','
Перевод: Она убедила меня попробовать снова.'),
('conversation','pretend','Завершить разговор и выразить мысль точнее
Пример: Do not pretend you understand.','
Перевод: Не притворяйтесь, что понимаете.'),
('conversation','assume','Завершить разговор и выразить мысль точнее
Пример: I assumed you knew.','
Перевод: Я предполагал, что вы знаете.'),
('conversation','prove','Завершить разговор и выразить мысль точнее
Пример: Can you prove it?','
Перевод: Можете это доказать?'),
('conversation','reassure','Завершить разговор и выразить мысль точнее
Пример: Your message reassured me.','
Перевод: Ваше сообщение меня успокоило.'),
('conversation','misunderstand','Завершить разговор и выразить мысль точнее
Пример: I think you misunderstood me.','
Перевод: Я думаю, вы меня неправильно поняли.'),
('conversation','misunderstanding','Завершить разговор и выразить мысль точнее
Пример: Sorry, it was a misunderstanding.','
Перевод: Извините, это было недоразумение.'),
('conversation','increase','Частые изменения и действия
Пример: The rent increased this year.','
Перевод: Арендная плата выросла в этом году.'),
('conversation','decrease','Частые изменения и действия
Пример: My travel costs decreased.','
Перевод: Мои дорожные расходы уменьшились.'),
('conversation','reduce','Частые изменения и действия
Пример: I want to reduce my expenses.','
Перевод: Я хочу сократить свои расходы.'),
('conversation','raise','Частые изменения и действия
Пример: Please raise your hand.','
Перевод: Пожалуйста, поднимите руку.'),
('conversation','lower','Частые изменения и действия
Пример: Could you lower your voice?','
Перевод: Не могли бы вы говорить потише?'),
('conversation','expand','Частые изменения и действия
Пример: I want to expand my vocabulary.','
Перевод: Я хочу расширить свой словарный запас.'),
('conversation','develop','Частые изменения и действия
Пример: I want to develop new skills.','
Перевод: Я хочу развивать новые навыки.'),
('conversation','adapt','Частые изменения и действия
Пример: It takes time to adapt.','
Перевод: Для адаптации требуется время.'),
('conversation','adjust','Частые изменения и действия
Пример: Can you adjust the seat?','
Перевод: Можете отрегулировать сиденье?'),
('conversation','recover','Частые изменения и действия
Пример: I need time to recover.','
Перевод: Мне нужно время на восстановление.'),
('conversation','settle','Частые изменения и действия
Пример: It took time to settle into the new job.','
Перевод: Понадобилось время, чтобы освоиться на новой работе.'),
('conversation','separate','Частые изменения и действия
Пример: Keep these documents separate.','
Перевод: Держите эти документы раздельно.'),
('conversation','combine','Частые изменения и действия
Пример: I combine work and study.','
Перевод: Я совмещаю работу и учёбу.'),
('conversation','divide','Частые изменения и действия
Пример: Divide the bill between us.','
Перевод: Разделим счёт между нами.'),
('conversation','count','Частые изменения и действия
Пример: Let me count the money.','
Перевод: Позвольте мне посчитать деньги.'),
('conversation','measure','Частые изменения и действия
Пример: Can you measure the room?','
Перевод: Можете измерить комнату?'),
('conversation','weigh','Частые изменения и действия
Пример: How much does this bag weigh?','
Перевод: Сколько весит эта сумка?'),
('conversation','calculate','Частые изменения и действия
Пример: Let me calculate the total.','
Перевод: Позвольте мне посчитать общую сумму.'),
('conversation','estimate','Частые изменения и действия
Пример: Can you give me an estimate?','
Перевод: Можете дать мне оценку?'),
('conversation','select','Частые изменения и действия
Пример: Select your language.','
Перевод: Выберите ваш язык.'),
('conversation','sort','Частые изменения и действия
Пример: I need to sort these papers.','
Перевод: Мне нужно рассортировать эти бумаги.'),
('conversation','suit','Частые изменения и действия
Пример: Does Friday suit you?','
Перевод: Подходит ли вам пятница?'),
('conversation','bend','Движение, тело и реакции
Пример: Bend your knees slowly.','
Перевод: Медленно согните колени.'),
('conversation','lean','Движение, тело и реакции
Пример: Do not lean on the door.','
Перевод: Не опирайтесь на дверь.'),
('conversation','roll','Движение, тело и реакции
Пример: Roll up your sleeves.','
Перевод: Засучите рукава.'),
('conversation','slide','Движение, тело и реакции
Пример: Slide the door to the left.','
Перевод: Сдвиньте дверь влево.'),
('conversation','slip','Движение, тело и реакции
Пример: Be careful not to slip.','
Перевод: Будьте осторожны, не поскользнитесь.'),
('conversation','stumble','Движение, тело и реакции
Пример: I stumbled on the stairs.','
Перевод: Я споткнулся на лестнице.'),
('conversation','crawl','Движение, тело и реакции
Пример: The baby is learning to crawl.','
Перевод: Малыш учится ползать.'),
('conversation','wave','Движение, тело и реакции
Пример: She waved goodbye.','
Перевод: Она помахала на прощание.'),
('conversation','nod','Движение, тело и реакции
Пример: He nodded to say yes.','
Перевод: Он кивнул, чтобы сказать «да».'),
('conversation','hug','Движение, тело и реакции
Пример: Can I give you a hug?','
Перевод: Можно я обниму вас?'),
('conversation','kiss','Движение, тело и реакции
Пример: She kissed the baby.','
Перевод: Она поцеловала ребёнка.'),
('conversation','cry','Движение, тело и реакции
Пример: The baby is crying.','
Перевод: Малыш плачет.'),
('conversation','shout','Движение, тело и реакции
Пример: You do not need to shout.','
Перевод: Вам не нужно кричать.'),
('conversation','whisper','Движение, тело и реакции
Пример: Please whisper. The baby is asleep.','
Перевод: Пожалуйста, говорите шёпотом. Малыш спит.'),
('conversation','sneeze','Движение, тело и реакции
Пример: I keep sneezing.','
Перевод: Я всё время чихаю.'),
('conversation','yawn','Движение, тело и реакции
Пример: I cannot stop yawning.','
Перевод: Я не могу перестать зевать.'),
('conversation','sweat','Движение, тело и реакции
Пример: I sweat a lot during exercise.','
Перевод: Я сильно потею во время упражнений.'),
('conversation','shiver','Движение, тело и реакции
Пример: I am shivering.','
Перевод: У меня дрожь.'),
('conversation','itch','Движение, тело и реакции
Пример: My skin itches.','
Перевод: У меня зудит кожа.'),
('conversation','swallow','Движение, тело и реакции
Пример: It hurts when I swallow.','
Перевод: Болит, когда я глотаю.'),
('conversation','chew','Движение, тело и реакции
Пример: Chew your food slowly.','
Перевод: Жуйте пищу медленно.'),
('conversation','blink','Движение, тело и реакции
Пример: Try blinking a few times.','
Перевод: Попробуйте несколько раз моргнуть.'),
('conversation','react','Движение, тело и реакции
Пример: How did she react?','
Перевод: Как она отреагировала?'),
('conversation','response','Движение, тело и реакции
Пример: I am waiting for a response.','
Перевод: Я жду ответа.'),
('conversation','village','Природа, погода и места вокруг
Пример: My parents live in a village.','
Перевод: Мои родители живут в деревне.'),
('conversation','suburb','Природа, погода и места вокруг
Пример: I live in a suburb.','
Перевод: Я живу в пригороде.'),
('conversation','countryside','Природа, погода и места вокруг
Пример: We spent the weekend in the countryside.','
Перевод: Мы провели выходные за городом.'),
('conversation','path','Природа, погода и места вокруг
Пример: Follow this path.','
Перевод: Идите по этой тропинке.'),
('conversation','trail','Природа, погода и места вокруг
Пример: This trail leads to the lake.','
Перевод: Эта тропа ведёт к озеру.'),
('conversation','desert','Природа, погода и места вокруг
Пример: It gets cold in the desert at night.','
Перевод: Ночью в пустыне становится холодно.'),
('conversation','sand','Природа, погода и места вокруг
Пример: There is sand in my shoes.','
Перевод: В моих ботинках песок.'),
('conversation','soil','Природа, погода и места вокруг
Пример: The soil is dry.','
Перевод: Почва сухая.'),
('conversation','leaf','Природа, погода и места вокруг
Пример: A leaf fell from the tree.','
Перевод: Лист упал с дерева.'),
('conversation','branch','Природа, погода и места вокруг
Пример: A branch fell on the road.','
Перевод: Ветка упала на дорогу.'),
('conversation','root','Природа, погода и места вокруг
Пример: The tree has deep roots.','
Перевод: У дерева глубокие корни.'),
('conversation','seed','Природа, погода и места вокруг
Пример: Plant the seeds in spring.','
Перевод: Посейте семена весной.'),
('conversation','plant','Природа, погода и места вокруг
Пример: This plant needs water.','
Перевод: Этому растению нужна вода.'),
('conversation','bush','Природа, погода и места вокруг
Пример: There is a bird in the bush.','
Перевод: В кусте есть птица.'),
('conversation','shade','Природа, погода и места вокруг
Пример: Let''s sit in the shade.','
Перевод: Сядем в тени.'),
('conversation','shadow','Природа, погода и места вокруг
Пример: I saw a shadow on the wall.','
Перевод: Я увидел тень на стене.'),
('conversation','sunlight','Природа, погода и места вокруг
Пример: This room gets a lot of sunlight.','
Перевод: В этой комнате много солнечного света.'),
('conversation','moon','Природа, погода и места вокруг
Пример: The moon is bright tonight.','
Перевод: Сегодня ночью луна яркая.'),
('conversation','star','Природа, погода и места вокруг
Пример: You can see the stars here.','
Перевод: Здесь можно увидеть звёзды.'),
('conversation','air','Природа, погода и места вокруг
Пример: I need some fresh air.','
Перевод: Мне нужен свежий воздух.'),
('conversation','breeze','Природа, погода и места вокруг
Пример: There is a nice breeze.','
Перевод: Дует приятный ветерок.'),
('conversation','thunder','Природа, погода и места вокруг
Пример: I can hear thunder.','
Перевод: Я слышу гром.'),
('conversation','lightning','Природа, погода и места вокруг
Пример: We saw lightning in the distance.','
Перевод: Мы видели молнию вдалеке.'),
('conversation','flood','Природа, погода и места вокруг
Пример: The flood damaged the road.','
Перевод: Наводнение повредило дорогу.'),
('conversation','earthquake','Природа, погода и места вокруг
Пример: There was an earthquake last night.','
Перевод: Вчера ночью было землетрясение.'),
('conversation','pollution','Природа, погода и места вокруг
Пример: Air pollution is a problem here.','
Перевод: Загрязнение воздуха здесь проблема.'),
('conversation','climate','Природа, погода и места вокруг
Пример: The climate is mild here.','
Перевод: Климат здесь умеренный.'),
('conversation','freezing','Природа, погода и места вокруг
Пример: It is freezing outside.','
Перевод: На улице морозно.'),
('conversation','boiling','Природа, погода и места вокруг
Пример: The water is boiling.','
Перевод: Вода кипит.'),
('conversation','slippery','Природа, погода и места вокруг
Пример: The road is slippery.','
Перевод: Дорога скользкая.'),
('conversation','supermarket','Повседневные места и услуги
Пример: I am going to the supermarket.','
Перевод: Я иду в супермаркет.'),
('conversation','bakery','Повседневные места и услуги
Пример: There is a bakery nearby.','
Перевод: Недалеко есть пекарня.'),
('conversation','butcher','Повседневные места и услуги
Пример: I buy meat from the local butcher.','
Перевод: Я покупаю мясо у местного мясника.'),
('conversation','hairdresser','Повседневные места и услуги
Пример: I have an appointment with the hairdresser.','
Перевод: У меня запись к парикмахеру.'),
('conversation','barber','Повседневные места и услуги
Пример: I need to find a barber.','
Перевод: Мне нужно найти барбера.'),
('conversation','salon','Повседневные места и услуги
Пример: The salon opens at ten.','
Перевод: Салон открывается в десять часов.'),
('conversation','haircut','Повседневные места и услуги
Пример: I need a haircut.','
Перевод: Мне нужна стрижка.'),
('conversation','trim','Повседневные места и услуги
Пример: Just trim the ends, please.','
Перевод: Подстригите только кончики, пожалуйста.'),
('conversation','shave','Повседневные места и услуги
Пример: I shave every morning.','
Перевод: Я бреюсь каждое утро.'),
('conversation','beard','Повседневные места и услуги
Пример: I want to trim my beard.','
Перевод: Я хочу подстричь бороду.'),
('conversation','post','Повседневные места и услуги
Пример: I sent it by post.','
Перевод: Я отправил это по почте.'),
('conversation','mail','Повседневные места и услуги
Пример: Has the mail arrived?','
Перевод: Пришла ли почта?'),
('conversation','parcel','Повседневные места и услуги
Пример: I am waiting for a parcel.','
Перевод: Я жду посылку.'),
('conversation','package','Повседневные места и услуги
Пример: My package has not arrived.','
Перевод: Моя посылка не пришла.'),
('conversation','envelope','Повседневные места и услуги
Пример: Put the letter in an envelope.','
Перевод: Положите письмо в конверт.'),
('conversation','letter','Повседневные места и услуги
Пример: I received a letter.','
Перевод: Я получил письмо.'),
('conversation','postcard','Повседневные места и услуги
Пример: I sent my family a postcard.','
Перевод: Я отправил семье открытку.'),
('conversation','courier','Повседневные места и услуги
Пример: The courier will arrive soon.','
Перевод: Курьер скоро прибудет.'),
('conversation','counter','Повседневные места и услуги
Пример: Pay at the counter.','
Перевод: Оплатите у кассы.'),
('conversation','cashier','Повседневные места и услуги
Пример: Ask the cashier.','
Перевод: Спросите у кассира.'),
('conversation','trolley','Повседневные места и услуги
Пример: Do you need a shopping trolley?','
Перевод: Вам нужна тележка для покупок?'),
('conversation','cart','Повседневные места и услуги
Пример: Put the bags in the cart.','
Перевод: Положите сумки в тележку.'),
('conversation','aisle','Повседневные места и услуги
Пример: Which aisle is the rice in?','
Перевод: В каком ряду находится рис?'),
('conversation','label','Повседневные места и услуги
Пример: Read the label first.','
Перевод: Сначала прочитайте этикетку.'),
('conversation','tag','Повседневные места и услуги
Пример: The price is on the tag.','
Перевод: Цена указана на бирке.'),
('conversation','packet','Повседневные места и услуги
Пример: A packet of biscuits, please.','
Перевод: Пакет печенья, пожалуйста.'),
('conversation','wrapper','Повседневные места и услуги
Пример: Put the wrapper in the bin.','
Перевод: Положите обёртку в мусорное ведро.'),
('conversation','container','Повседневные места и услуги
Пример: Put the food in a container.','
Перевод: Положите еду в контейнер.'),
('conversation','cherry','Еда, заказ и предпочтения подробнее
Пример: These cherries are delicious.','
Перевод: Эти вишни вкусные.'),
('conversation','melon','Еда, заказ и предпочтения подробнее
Пример: I cut the melon in half.','
Перевод: Я разрезал дыню пополам.'),
('conversation','watermelon','Еда, заказ и предпочтения подробнее
Пример: We shared a watermelon.','
Перевод: Мы поделились арбузом.'),
('conversation','pineapple','Еда, заказ и предпочтения подробнее
Пример: Is the pineapple fresh?','
Перевод: Ананас свежий?'),
('conversation','avocado','Еда, заказ и предпочтения подробнее
Пример: I would like avocado on toast.','
Перевод: Я бы хотел авокадо на тосте.'),
('conversation','spinach','Еда, заказ и предпочтения подробнее
Пример: Add some spinach to the soup.','
Перевод: Добавьте немного шпината в суп.'),
('conversation','broccoli','Еда, заказ и предпочтения подробнее
Пример: Would you like broccoli with that?','
Перевод: Хотите брокколи к этому?'),
('conversation','pumpkin','Еда, заказ и предпочтения подробнее
Пример: I made pumpkin soup.','
Перевод: Я приготовил тыквенный суп.'),
('conversation','corn','Еда, заказ и предпочтения подробнее
Пример: I like corn in my salad.','
Перевод: Мне нравится кукуруза в салате.'),
('conversation','oat','Еда, заказ и предпочтения подробнее
Пример: Do you have oat milk?','
Перевод: У вас есть овсяное молоко?'),
('conversation','porridge','Еда, заказ и предпочтения подробнее
Пример: I eat porridge for breakfast.','
Перевод: Я ем кашу на завтрак.'),
('conversation','toast','Еда, заказ и предпочтения подробнее
Пример: Toast with butter, please.','
Перевод: Тост с маслом, пожалуйста.'),
('conversation','biscuit','Еда, заказ и предпочтения подробнее
Пример: Would you like a biscuit with your tea?','
Перевод: Хотите печенье к чаю?'),
('conversation','pie','Еда, заказ и предпочтения подробнее
Пример: I made an apple pie.','
Перевод: Я испек яблочный пирог.'),
('conversation','pancake','Еда, заказ и предпочтения подробнее
Пример: I would like pancakes for breakfast.','
Перевод: Я бы хотел блины на завтрак.'),
('conversation','dessert','Еда, заказ и предпочтения подробнее
Пример: Would you like dessert?','
Перевод: Хотите десерт?'),
('conversation','snack','Еда, заказ и предпочтения подробнее
Пример: I need a small snack.','
Перевод: Мне нужен небольшой перекус.'),
('conversation','takeaway','Еда, заказ и предпочтения подробнее
Пример: Let''s get a takeaway.','
Перевод: Давайте возьмём еду навынос.'),
('conversation','takeout','Еда, заказ и предпочтения подробнее
Пример: We ordered takeout.','
Перевод: Мы заказали еду навынос.'),
('conversation','homemade','Еда, заказ и предпочтения подробнее
Пример: This bread is homemade.','
Перевод: Этот хлеб домашний.'),
('conversation','grilled','Еда, заказ и предпочтения подробнее
Пример: I would like grilled fish.','
Перевод: Я бы хотел рыбу на гриле.'),
('conversation','fried','Еда, заказ и предпочтения подробнее
Пример: Fried eggs, please.','
Перевод: Жареные яйца, пожалуйста.'),
('conversation','roasted','Еда, заказ и предпочтения подробнее
Пример: We had roasted vegetables.','
Перевод: Мы ели запечённые овощи.'),
('conversation','steamed','Еда, заказ и предпочтения подробнее
Пример: I prefer steamed vegetables.','
Перевод: Я предпочитаю паровые овощи.'),
('conversation','frozen','Еда, заказ и предпочтения подробнее
Пример: Can I use frozen vegetables?','
Перевод: Можно использовать замороженные овощи?'),
('conversation','canned','Еда, заказ и предпочтения подробнее
Пример: I use canned tomatoes.','
Перевод: Я использую консервированные помидоры.'),
('conversation','organic','Еда, заказ и предпочтения подробнее
Пример: Do you sell organic milk?','
Перевод: Вы продаёте органическое молоко?'),
('conversation','decaf','Еда, заказ и предпочтения подробнее
Пример: Can I have decaf coffee?','
Перевод: Можно мне кофе без кофеина?'),
('conversation','item','Вещи, оценки и повседневные ситуации
Пример: This item is out of stock.','
Перевод: Этот товар закончился.'),
('conversation','object','Вещи, оценки и повседневные ситуации
Пример: There is a small object on the floor.','
Перевод: На полу лежит небольшой предмет.'),
('conversation','stuff','Вещи, оценки и повседневные ситуации
Пример: Where can I put my stuff?','
Перевод: Куда я могу положить свои вещи?'),
('conversation','belongings','Вещи, оценки и повседневные ситуации
Пример: Do not leave your belongings here.','
Перевод: Не оставляйте здесь свои вещи.'),
('conversation','property','Вещи, оценки и повседневные ситуации
Пример: This is private property.','
Перевод: Это частная собственность.'),
('conversation','owner','Вещи, оценки и повседневные ситуации
Пример: Who is the owner?','
Перевод: Кто владелец?'),
('conversation','access','Вещи, оценки и повседневные ситуации
Пример: Do I have access to the internet?','
Перевод: Есть ли у меня доступ к интернету?'),
('conversation','membership','Вещи, оценки и повседневные ситуации
Пример: How much is gym membership?','
Перевод: Сколько стоит абонемент в спортзал?'),
('conversation','entry','Вещи, оценки и повседневные ситуации
Пример: Is entry free?','
Перевод: Вход бесплатный?'),
('conversation','admission','Вещи, оценки и повседневные ситуации
Пример: What is the admission price?','
Перевод: Какова цена входного билета?'),
('conversation','opening','Вещи, оценки и повседневные ситуации
Пример: What are your opening hours?','
Перевод: Каковы ваши часы работы?'),
('conversation','closing','Вещи, оценки и повседневные ситуации
Пример: What time is closing?','
Перевод: Во сколько закрывается?'),
('conversation','working','Вещи, оценки и повседневные ситуации
Пример: The lift is not working.','
Перевод: Лифт не работает.'),
('conversation','faulty','Вещи, оценки и повседневные ситуации
Пример: I received a faulty charger.','
Перевод: Я получил неисправный зарядный адаптер.'),
('conversation','warranty','Вещи, оценки и повседневные ситуации
Пример: Is it still under warranty?','
Перевод: Он всё ещё на гарантии?'),
('conversation','guarantee','Вещи, оценки и повседневные ситуации
Пример: Does it come with a guarantee?','
Перевод: Есть ли гарантия?'),
('conversation','replacement','Вещи, оценки и повседневные ситуации
Пример: I would like a replacement.','
Перевод: Я бы хотел замену.'),
('conversation','complaint','Вещи, оценки и повседневные ситуации
Пример: I want to make a complaint.','
Перевод: Я хочу подать жалобу.'),
('conversation','inconvenience','Вещи, оценки и повседневные ситуации
Пример: Sorry for the inconvenience.','
Перевод: Извините за причинённые неудобства.'),
('conversation','apology','Вещи, оценки и повседневные ситуации
Пример: Please accept my apology.','
Перевод: Примите, пожалуйста, мои извинения.'),
('conversation','compensation','Вещи, оценки и повседневные ситуации
Пример: Can I claim compensation?','
Перевод: Можно ли потребовать компенсацию?'),
('conversation','cancellation','Вещи, оценки и повседневные ситуации
Пример: Is there a cancellation fee?','
Перевод: Есть ли плата за отмену?'),
('conversation','confirmation','Вещи, оценки и повседневные ситуации
Пример: I received a booking confirmation.','
Перевод: Я получил подтверждение бронирования.'),
('conversation','duration','Длительность, планы и изменения
Пример: What is the duration of the course?','
Перевод: Какова продолжительность курса?'),
('conversation','period','Длительность, планы и изменения
Пример: There is a waiting period.','
Перевод: Есть период ожидания.'),
('conversation','moment','Длительность, планы и изменения
Пример: Just a moment, please.','
Перевод: Минуточку, пожалуйста.'),
('conversation','instance','Длительность, планы и изменения
Пример: For instance, we could take the train.','
Перевод: Например, мы могли бы поехать поездом.'),
('conversation','occasion','Длительность, планы и изменения
Пример: What is the occasion?','
Перевод: Какой повод?'),
('conversation','nowadays','Длительность, планы и изменения
Пример: I mostly work from home nowadays.','
Перевод: В последнее время я в основном работаю из дома.'),
('conversation','meantime','Длительность, планы и изменения
Пример: In the meantime, have a seat.','
Перевод: Тем временем, присаживайтесь.'),
('conversation','meanwhile','Длительность, планы и изменения
Пример: Meanwhile, I will make tea.','
Перевод: Тем временем я сделаю чай.'),
('conversation','overnight','Длительность, планы и изменения
Пример: Can I leave the car here overnight?','
Перевод: Можно ли оставить машину здесь на ночь?'),
('conversation','beforehand','Длительность, планы и изменения
Пример: Let me know beforehand.','
Перевод: Сообщите мне заранее.'),
('conversation','afterwards','Длительность, планы и изменения
Пример: We went for a walk afterwards.','
Перевод: Мы потом пошли гулять.'),
('conversation','forever','Длительность, планы и изменения
Пример: I will not stay here forever.','
Перевод: Я не буду здесь оставаться вечно.'),
('conversation','ago','Длительность, планы и изменения
Пример: I moved here two years ago.','
Перевод: Я переехал сюда два года назад.'),
('conversation','towards','Длительность, планы и изменения
Пример: Walk towards the station.','
Перевод: Идите к станции.'),
('conversation','plus','Длительность, планы и изменения
Пример: It costs ten euros plus delivery.','
Перевод: Это стоит десять евро плюс доставка.'),
('conversation','minus','Длительность, планы и изменения
Пример: It is minus five outside.','
Перевод: На улице минус пять градусов.'),
('conversation','percent','Длительность, планы и изменения
Пример: There is a ten percent discount.','
Перевод: Есть скидка в десять процентов.'),
('conversation','percentage','Длительность, планы и изменения
Пример: What percentage do I pay?','
Перевод: Какой процент я должен заплатить?'),
('conversation','maximum','Длительность, планы и изменения
Пример: What is the maximum weight?','
Перевод: Какой максимальный вес?'),
('conversation','minimum','Длительность, планы и изменения
Пример: Is there a minimum charge?','
Перевод: Есть ли минимальная плата?'),
('conversation','average','Длительность, планы и изменения
Пример: What is the average price?','
Перевод: Какая средняя цена?'),
('conversation','roughly','Длительность, планы и изменения
Пример: It takes roughly an hour.','
Перевод: Это займет примерно час.'),
('conversation','approximately','Длительность, планы и изменения
Пример: It costs approximately twenty euros.','
Перевод: Это стоит примерно двадцать евро.'),
('conversation','plenty','Длительность, планы и изменения
Пример: We have plenty of time.','
Перевод: У нас достаточно времени.'),
('conversation','several','Длительность, планы и изменения
Пример: I have several questions.','
Перевод: У меня несколько вопросов.'),
('conversation','various','Длительность, планы и изменения
Пример: We discussed various options.','
Перевод: Мы обсудили разные варианты.'),
('conversation','additional','Длительность, планы и изменения
Пример: Are there any additional costs?','
Перевод: Есть ли дополнительные расходы?'),
('medicine','consultation','Приём: установить контакт и уточнить жалобу
Пример: The consultation will take about twenty minutes.','
Перевод: Консультация займет около двадцати минут.'),
('medicine','patient','Приём: установить контакт и уточнить жалобу
Пример: How would the patient like to be addressed?','
Перевод: Как пациент предпочитает, чтобы к нему обращались?'),
('medicine','clinician','Приём: установить контакт и уточнить жалобу
Пример: The clinician will review the results.','
Перевод: Врач ознакомится с результатами.'),
('medicine','physician','Приём: установить контакт и уточнить жалобу
Пример: Please contact the referring physician.','
Перевод: Пожалуйста, свяжитесь с врачом, выдавшим направление.'),
('medicine','general practitioner','Приём: установить контакт и уточнить жалобу
Пример: Your general practitioner can arrange follow-up.','
Перевод: Ваш терапевт может организовать последующее наблюдение.'),
('medicine','specialist','Приём: установить контакт и уточнить жалобу
Пример: I would like to refer you to a specialist.','
Перевод: Я хотел бы направить вас к специалисту.'),
('medicine','referral','Приём: установить контакт и уточнить жалобу
Пример: We have received your referral.','
Перевод: Мы получили ваше направление.'),
('medicine','appointment','Приём: установить контакт и уточнить жалобу
Пример: Is this your first appointment here?','
Перевод: Это ваш первый приём здесь?'),
('medicine','chief complaint','Приём: установить контакт и уточнить жалобу
Пример: What is the patient''s chief complaint?','
Перевод: Какова главная жалоба пациента?'),
('medicine','presenting complaint','Приём: установить контакт и уточнить жалобу
Пример: Document the presenting complaint in the patient''s words.','
Перевод: Запишите основную жалобу словами пациента.'),
('medicine','concern','Приём: установить контакт и уточнить жалобу
Пример: What concerns you most about this?','
Перевод: Что вас больше всего беспокоит в этом?'),
('medicine','onset','Приём: установить контакт и уточнить жалобу
Пример: Was the onset sudden or gradual?','
Перевод: Симптомы появились внезапно или постепенно?'),
('medicine','duration','Приём: установить контакт и уточнить жалобу
Пример: What is the duration of the pain?','
Перевод: Какова продолжительность боли?'),
('medicine','frequency','Приём: установить контакт и уточнить жалобу
Пример: How often does this happen?','
Перевод: Как часто это происходит?'),
('medicine','severity','Приём: установить контакт и уточнить жалобу
Пример: How would you describe the severity?','
Перевод: Как бы вы оценили тяжесть?'),
('medicine','location','Приём: установить контакт и уточнить жалобу
Пример: Can you point to the location of the pain?','
Перевод: Укажите, где именно болит.'),
('medicine','radiation','Приём: установить контакт и уточнить жалобу
Пример: Is there any radiation of the pain?','
Перевод: Есть ли радиация боли?'),
('medicine','trigger','Приём: установить контакт и уточнить жалобу
Пример: Have you noticed any triggers?','
Перевод: Вы заметили какие‑либо провоцирующие факторы?'),
('medicine','aggravating factor','Приём: установить контакт и уточнить жалобу
Пример: What aggravating factors have you noticed?','
Перевод: Какие факторы усиливают боль?'),
('medicine','relieving factor','Приём: установить контакт и уточнить жалобу
Пример: Are there any relieving factors?','
Перевод: Есть ли факторы, облегчающие боль?'),
('medicine','intermittent','Приём: установить контакт и уточнить жалобу
Пример: Is the pain intermittent or constant?','
Перевод: Боль прерывистая или постоянная?'),
('medicine','constant','Приём: установить контакт и уточнить жалобу
Пример: Has the pain been constant?','
Перевод: Была ли боль постоянной?'),
('medicine','acute','Приём: установить контакт и уточнить жалобу
Пример: The patient reports acute abdominal pain.','
Перевод: Пациент сообщает об острой боли в животе.'),
('medicine','chronic','Приём: установить контакт и уточнить жалобу
Пример: How does chronic pain affect your daily life?','
Перевод: Как хроническая боль влияет на вашу повседневную жизнь?'),
('medicine','progressive','Приём: установить контакт и уточнить жалобу
Пример: The patient describes progressive weakness.','
Перевод: Пациент описывает прогрессирующую слабость.'),
('medicine','recurrent','Приём: установить контакт и уточнить жалобу
Пример: Have you had recurrent infections?','
Перевод: Были ли у вас повторяющиеся инфекции?'),
('medicine','sudden','Приём: установить контакт и уточнить жалобу
Пример: Was there a sudden change?','
Перевод: Было ли резкое изменение?'),
('medicine','gradual','Приём: установить контакт и уточнить жалобу
Пример: Was the onset gradual?','
Перевод: Симптомы появлялись постепенно?'),
('medicine','persistent','Приём: установить контакт и уточнить жалобу
Пример: How long has the persistent cough lasted?','
Перевод: Как долго продолжается стойкий кашель?'),
('medicine','associated symptom','Приём: установить контакт и уточнить жалобу
Пример: Are there any associated symptoms?','
Перевод: Есть ли сопутствующие симптомы?'),
('medicine','medical history','Анамнез и лекарства
Пример: Could you tell me about your medical history?','
Перевод: Расскажите, пожалуйста, о своей медицинской истории.'),
('medicine','past medical history','Анамнез и лекарства
Пример: Let us review your past medical history.','
Перевод: Давайте рассмотрим вашу анамнез заболевания.'),
('medicine','surgical history','Анамнез и лекарства
Пример: What is your surgical history?','
Перевод: Какова ваша хирургическая история?'),
('medicine','family history','Анамнез и лекарства
Пример: Is there a family history of diabetes?','
Перевод: Есть ли в семье случаи диабета?'),
('medicine','social history','Анамнез и лекарства
Пример: The social history includes occupation and living conditions.','
Перевод: Социальный анамнез включает профессию и условия проживания.'),
('medicine','medication history','Анамнез и лекарства
Пример: Please confirm the medication history.','
Перевод: Подтвердите, пожалуйста, историю приёма лекарств.'),
('medicine','allergy','Анамнез и лекарства
Пример: Do you have any known allergies?','
Перевод: Есть ли у вас известные аллергии?'),
('medicine','adverse reaction','Анамнез и лекарства
Пример: Have you had an adverse reaction to a medicine?','
Перевод: Была ли у вас реакция на лекарство?'),
('medicine','side effect','Анамнез и лекарства
Пример: Have you noticed any side effects?','
Перевод: Вы заметили какие‑либо побочные эффекты?'),
('medicine','medication','Анамнез и лекарства
Пример: What medications are you taking?','
Перевод: Какие лекарства вы принимаете?'),
('medicine','prescription','Анамнез и лекарства
Пример: Do you have a copy of your prescription?','
Перевод: У вас есть копия вашего рецепта?'),
('medicine','over-the-counter','Анамнез и лекарства
Пример: Do you take any over-the-counter medicines?','
Перевод: Вы принимаете какие‑либо безрецептурные препараты?'),
('medicine','supplement','Анамнез и лекарства
Пример: Do you take any supplements?','
Перевод: Вы принимаете какие‑либо добавки?'),
('medicine','dose','Анамнез и лекарства
Пример: What dose are you currently taking?','
Перевод: Какую дозу вы сейчас принимаете?'),
('medicine','dosage','Анамнез и лекарства
Пример: Let us check the prescribed dosage.','
Перевод: Давайте проверим назначенную дозу.'),
('medicine','route','Анамнез и лекарства
Пример: Please confirm the route of administration.','
Перевод: Пожалуйста, подтвердите путь введения препарата.'),
('medicine','oral','Анамнез и лекарства
Пример: Is this medication for oral use?','
Перевод: Этот препарат предназначен для приёма внутрь?'),
('medicine','topical','Анамнез и лекарства
Пример: This is listed as a topical medication.','
Перевод: Это указано как местный препарат.'),
('medicine','intravenous','Анамнез и лекарства
Пример: The chart records intravenous administration.','
Перевод: Медицинская карта фиксирует внутривенное введение.'),
('medicine','intramuscular','Анамнез и лекарства
Пример: Please check the documented intramuscular route.','
Перевод: Проверьте задокументированный внутримышечный путь.'),
('medicine','subcutaneous','Анамнез и лекарства
Пример: The prescription specifies subcutaneous administration.','
Перевод: В рецепте указано подкожное введение.'),
('medicine','inhaler','Анамнез и лекарства
Пример: Could you show me how you use your inhaler?','
Перевод: Можете показать, как вы используете ингалятор?'),
('medicine','adherence','Анамнез и лекарства
Пример: Are there any difficulties with medication adherence?','
Перевод: Есть ли трудности с соблюдением режима приёма?'),
('medicine','missed dose','Анамнез и лекарства
Пример: Have you had any missed doses?','
Перевод: Были ли пропущенные дозы?'),
('medicine','contraindication','Анамнез и лекарства
Пример: Check the documented contraindications.','
Перевод: Проверьте задокументированные противопоказания.'),
('medicine','interaction','Анамнез и лекарства
Пример: The pharmacist will check for interactions.','
Перевод: Фармацевт проверит возможные взаимодействия.'),
('medicine','reconciliation','Анамнез и лекарства
Пример: Medication reconciliation is part of admission.','
Перевод: Согласование лекарств является частью госпитализации.'),
('medicine','anticoagulant','Анамнез и лекарства
Пример: Are you taking an anticoagulant?','
Перевод: Вы принимаете антикоагулянт?'),
('medicine','antibiotic','Анамнез и лекарства
Пример: Have you recently taken any antibiotics?','
Перевод: Вы недавно принимали антибиотики?'),
('medicine','analgesic','Анамнез и лекарства
Пример: Which analgesic have you used?','
Перевод: Какой обезболивающий препарат вы использовали?'),
('medicine','pain','Боль и общие симптомы
Пример: Can you describe the pain in your own words?','
Перевод: Опишите боль своими словами, пожалуйста.'),
('medicine','ache','Боль и общие симптомы
Пример: Is it a dull ache?','
Перевод: Это тупая ноющая боль?'),
('medicine','sharp','Боль и общие симптомы
Пример: Is the pain sharp?','
Перевод: Боль острая?'),
('medicine','dull','Боль и общие симптомы
Пример: Would you describe it as a dull pain?','
Перевод: Вы бы назвали её тупой болью?'),
('medicine','burning','Боль и общие симптомы
Пример: Do you feel a burning sensation?','
Перевод: Вы чувствуете жжение?'),
('medicine','throbbing','Боль и общие симптомы
Пример: Is the headache throbbing?','
Перевод: Головная боль пульсирующая?'),
('medicine','cramping','Боль и общие симптомы
Пример: Is the pain cramping?','
Перевод: Боль схваткообразная?'),
('medicine','stabbing','Боль и общие симптомы
Пример: Does the pain feel stabbing?','
Перевод: Боль ощущается как колющая?'),
('medicine','tenderness','Боль и общие симптомы
Пример: There is tenderness on palpation.','
Перевод: При пальпации ощущается болезненность.'),
('medicine','swelling','Боль и общие симптомы
Пример: When did you first notice the swelling?','
Перевод: Когда вы впервые заметили отёк?'),
('medicine','edema','Боль и общие симптомы
Пример: The examination notes bilateral leg edema.','
Перевод: В осмотре отмечён двусторонний отёк ног.'),
('medicine','fatigue','Боль и общие симптомы
Пример: How has fatigue affected your activities?','
Перевод: Как усталость влияет на вашу активность?'),
('medicine','malaise','Боль и общие симптомы
Пример: The patient describes general malaise.','
Перевод: Пациент описывает общее недомогание.'),
('medicine','fever','Боль и общие симптомы
Пример: Have you measured your temperature during the fever?','
Перевод: Вы измеряли температуру во время лихорадки?'),
('medicine','chills','Боль и общие симптомы
Пример: Have you had chills?','
Перевод: У вас были озноб?'),
('medicine','night sweats','Боль и общие симптомы
Пример: Have you noticed night sweats?','
Перевод: Вы замечали ночные поты?'),
('medicine','weight loss','Боль и общие симптомы
Пример: Has the weight loss been intentional?','
Перевод: Потеря веса была намеренной?'),
('medicine','loss of appetite','Боль и общие симптомы
Пример: When did the loss of appetite begin?','
Перевод: Когда начался отказ от еды?'),
('medicine','nausea','Боль и общие симптомы
Пример: Do you feel nausea now?','
Перевод: Сейчас чувствуете тошноту?'),
('medicine','vomiting','Боль и общие симптомы
Пример: How many episodes of vomiting have you had?','
Перевод: Сколько раз вы рвали?'),
('medicine','dizziness','Боль и общие симптомы
Пример: What do you mean by dizziness?','
Перевод: Что вы имеете в виду под головокружением?'),
('medicine','vertigo','Боль и общие симптомы
Пример: Does the vertigo feel like the room is spinning?','
Перевод: При головокружении кажется, что комната вращается?'),
('medicine','syncope','Боль и общие симптомы
Пример: Was the episode of syncope witnessed?','
Перевод: Был ли эпизод обморока наблюдаемым?'),
('medicine','fainting','Боль и общие симптомы
Пример: Have you had any fainting episodes?','
Перевод: Бывали ли у вас случаи обмороков?'),
('medicine','numbness','Боль и общие симптомы
Пример: Where do you feel numbness?','
Перевод: Где вы чувствуете онемение?'),
('medicine','tingling','Боль и общие симптомы
Пример: Is there tingling in your fingers?','
Перевод: Есть ли покалывание в пальцах?'),
('medicine','weakness','Боль и общие симптомы
Пример: Is the weakness on one side or both?','
Перевод: Слабость проявляется с одной стороны или с обеих?'),
('medicine','rash','Боль и общие симптомы
Пример: When did the rash appear?','
Перевод: Когда появилось сыпь?'),
('medicine','itching','Боль и общие симптомы
Пример: Does the itching keep you awake?','
Перевод: Зуд мешает вам спать?'),
('medicine','bleeding','Боль и общие симптомы
Пример: When did the bleeding start?','
Перевод: Когда началось кровотечение?'),
('medicine','chest pain','Сердце и дыхание
Пример: When did the chest pain start?','
Перевод: Когда началась боль в груди?'),
('medicine','shortness of breath','Сердце и дыхание
Пример: Do you have shortness of breath at rest?','
Перевод: Есть ли одышка в покое?'),
('medicine','dyspnea','Сердце и дыхание
Пример: Document whether the dyspnea occurs on exertion.','
Перевод: Задокументируйте, возникает ли одышка при нагрузке.'),
('medicine','exertion','Сердце и дыхание
Пример: Do symptoms occur with exertion?','
Перевод: Возникают ли симптомы при нагрузке?'),
('medicine','orthopnea','Сердце и дыхание
Пример: Ask about orthopnea when taking the history.','
Перевод: Спросите о ортостатическом дыхании при сборе анамнеза.'),
('medicine','palpitations','Сердце и дыхание
Пример: How long do the palpitations last?','
Перевод: Как долго продолжаются палпитации?'),
('medicine','cough','Сердце и дыхание
Пример: Is the cough dry or productive?','
Перевод: Кашель сухой или продуктивный?'),
('medicine','productive cough','Сердце и дыхание
Пример: How long have you had a productive cough?','
Перевод: Как долго у вас продуктивный кашель?'),
('medicine','sputum','Сердце и дыхание
Пример: Have you noticed a change in the sputum?','
Перевод: Заметили ли вы изменения в мокроте?'),
('medicine','hemoptysis','Сердце и дыхание
Пример: The history includes an episode of hemoptysis.','
Перевод: В анамнезе отмечен эпизод гемоптизиса.'),
('medicine','wheeze','Сердце и дыхание
Пример: Have you noticed any wheeze?','
Перевод: Заметили ли вы хрипы?'),
('medicine','stridor','Сердце и дыхание
Пример: Document the presence of stridor.','
Перевод: Отметьте наличие стридора.'),
('medicine','respiratory rate','Сердце и дыхание
Пример: Record the respiratory rate.','
Перевод: Запишите частоту дыхания.'),
('medicine','oxygen saturation','Сердце и дыхание
Пример: Record the oxygen saturation and oxygen support.','
Перевод: Запишите сатурацию кислорода и поддержку кислородом.'),
('medicine','blood pressure','Сердце и дыхание
Пример: May I check your blood pressure?','
Перевод: Можно измерить ваше артериальное давление?'),
('medicine','heart rate','Сердце и дыхание
Пример: Record the heart rate.','
Перевод: Запишите частоту сердечных сокращений.'),
('medicine','pulse','Сердце и дыхание
Пример: I am going to check your pulse.','
Перевод: Я собираюсь проверить ваш пульс.'),
('medicine','rhythm','Сердце и дыхание
Пример: The report describes an irregular rhythm.','
Перевод: В отчёте описан нерегулярный ритм.'),
('medicine','arrhythmia','Сердце и дыхание
Пример: Is there a history of arrhythmia?','
Перевод: Есть ли в анамнезе аритмия?'),
('medicine','hypertension','Сердце и дыхание
Пример: Have you been diagnosed with hypertension?','
Перевод: Был ли вам поставлен диагноз гипертония?'),
('medicine','high blood pressure','Сердце и дыхание
Пример: Have you ever been told you have high blood pressure?','
Перевод: Говорили ли вам когда‑нибудь, что у вас повышенное давление?'),
('medicine','hypotension','Сердце и дыхание
Пример: The handover mentions hypotension.','
Перевод: В передаче упомянута гипотензия.'),
('medicine','heart failure','Сердце и дыхание
Пример: The referral notes a history of heart failure.','
Перевод: В направлении указана история сердечной недостаточности.'),
('medicine','myocardial infarction','Сердце и дыхание
Пример: The discharge summary records a previous myocardial infarction.','
Перевод: В выписном эпикризе зафиксирован предыдущий инфаркт миокарда.'),
('medicine','heart attack','Сердце и дыхание
Пример: Have you ever had a heart attack?','
Перевод: Был ли у вас когда‑нибудь инфаркт?'),
('medicine','stroke','Сердце и дыхание
Пример: Is there a family history of stroke?','
Перевод: Есть ли в семье случаи инсульта?'),
('medicine','asthma','Сердце и дыхание
Пример: How does asthma affect your daily activities?','
Перевод: Как астма влияет на вашу повседневную активность?'),
('medicine','pneumonia','Сердце и дыхание
Пример: The referral lists pneumonia as a possible diagnosis.','
Перевод: В направлении указана пневмония как возможный диагноз.'),
('medicine','pulmonary embolism','Сердце и дыхание
Пример: Pulmonary embolism is included in the differential diagnosis.','
Перевод: Тромбоэмболия лёгочной артерии включена в дифференциальный диагноз.'),
('medicine','electrocardiogram','Сердце и дыхание
Пример: I will explain what an electrocardiogram involves.','
Перевод: Я объясню, что включает в себя электрокардиограмма.'),
('medicine','abdomen','Живот, мочеиспускание и обмен веществ
Пример: May I examine your abdomen?','
Перевод: Можно осмотреть ваш живот?'),
('medicine','abdominal pain','Живот, мочеиспускание и обмен веществ
Пример: Can you point to the abdominal pain?','
Перевод: Укажите, пожалуйста, место болей в животе.'),
('medicine','bloating','Живот, мочеиспускание и обмен веществ
Пример: When do you notice the bloating?','
Перевод: Когда вы замечаете вздутие?'),
('medicine','heartburn','Живот, мочеиспускание и обмен веществ
Пример: How often do you get heartburn?','
Перевод: Как часто у вас появляется изжога?'),
('medicine','dysphagia','Живот, мочеиспускание и обмен веществ
Пример: Is the dysphagia associated with solids or liquids?','
Перевод: Дисфагия возникает при приёме твёрдой пищи или жидкостей?'),
('medicine','diarrhea','Живот, мочеиспускание и обмен веществ
Пример: When did the diarrhea begin?','
Перевод: Когда началась диарея?'),
('medicine','constipation','Живот, мочеиспускание и обмен веществ
Пример: How long have you had constipation?','
Перевод: Как долго у вас запор?'),
('medicine','bowel movement','Живот, мочеиспускание и обмен веществ
Пример: When was your last bowel movement?','
Перевод: Когда был ваш последний стул?'),
('medicine','stool','Живот, мочеиспускание и обмен веществ
Пример: Have you noticed blood in your stool?','
Перевод: Вы замечали кровь в стуле?'),
('medicine','melena','Живот, мочеиспускание и обмен веществ
Пример: Ask whether the patient has noticed melena.','
Перевод: Спросите, замечал ли пациент мелена.'),
('medicine','hematemesis','Живот, мочеиспускание и обмен веществ
Пример: Document any reported hematemesis.','
Перевод: Задокументируйте любой сообщённый гематемезис.'),
('medicine','jaundice','Живот, мочеиспускание и обмен веществ
Пример: Has anyone noticed jaundice or yellowing of your eyes?','
Перевод: Кто‑то замечал желтушность или пожелтение глаз?'),
('medicine','liver','Живот, мочеиспускание и обмен веществ
Пример: The report mentions the liver.','
Перевод: В отчёте упоминается печень.'),
('medicine','gallbladder','Живот, мочеиспускание и обмен веществ
Пример: The scan includes the gallbladder.','
Перевод: В сканировании включён желчный пузырь.'),
('medicine','pancreas','Живот, мочеиспускание и обмен веществ
Пример: The pancreas is described in the imaging report.','
Перевод: Поджелудочная железа описана в рентгеновском отчёте.'),
('medicine','kidney','Живот, мочеиспускание и обмен веществ
Пример: Have you had kidney problems before?','
Перевод: Были ли у вас ранее проблемы с почками?'),
('medicine','urine','Живот, мочеиспускание и обмен веществ
Пример: We need a urine sample.','
Перевод: Нужен образец мочи.'),
('medicine','urination','Живот, мочеиспускание и обмен веществ
Пример: Have you noticed pain during urination?','
Перевод: Вы замечали боль при мочеиспускании?'),
('medicine','dysuria','Живот, мочеиспускание и обмен веществ
Пример: Ask about dysuria and urinary frequency.','
Перевод: Спросите о дизурии и частоте мочеиспускания.'),
('medicine','hematuria','Живот, мочеиспускание и обмен веществ
Пример: Has the patient noticed visible hematuria?','
Перевод: Пациент замечал видимую гематурию?'),
('medicine','incontinence','Живот, мочеиспускание и обмен веществ
Пример: Would you like to discuss the incontinence?','
Перевод: Хотите обсудить проблему недержания?'),
('medicine','retention','Живот, мочеиспускание и обмен веществ
Пример: The referral mentions urinary retention.','
Перевод: В направлении указано задержка мочи.'),
('medicine','urinary tract infection','Живот, мочеиспускание и обмен веществ
Пример: Have you had a urinary tract infection before?','
Перевод: Были ли у вас ранее инфекции мочевых путей?'),
('medicine','diabetes','Живот, мочеиспускание и обмен веществ
Пример: How long have you had diabetes?','
Перевод: Как давно у вас диабет?'),
('medicine','blood glucose','Живот, мочеиспускание и обмен веществ
Пример: When was your blood glucose last checked?','
Перевод: Когда в последний раз проверяли уровень глюкозы в крови?'),
('medicine','hypoglycemia','Живот, мочеиспускание и обмен веществ
Пример: Have you had episodes of hypoglycemia?','
Перевод: Бывали ли у вас эпизоды гипогликемии?'),
('medicine','hyperglycemia','Живот, мочеиспускание и обмен веществ
Пример: The notes mention hyperglycemia.','
Перевод: В записях упоминается гипергликемия.'),
('medicine','thyroid','Живот, мочеиспускание и обмен веществ
Пример: Have you had thyroid problems?','
Перевод: Были ли у вас проблемы с щитовидной железой?'),
('medicine','dehydration','Живот, мочеиспускание и обмен веществ
Пример: The examination will assess for dehydration.','
Перевод: Осмотр будет включать оценку обезвоживания.'),
('medicine','fluid intake','Живот, мочеиспускание и обмен веществ
Пример: How has your fluid intake been today?','
Перевод: Каково было ваше потребление жидкости сегодня?'),
('medicine','physical examination','Осмотр и обследования
Пример: I would like to perform a physical examination.','
Перевод: Я хотел бы провести физический осмотр.'),
('medicine','consent','Осмотр и обследования
Пример: May I ask for your consent to examine you?','
Перевод: Можно получить ваше согласие на осмотр?'),
('medicine','chaperone','Осмотр и обследования
Пример: Would you like a chaperone present?','
Перевод: Хотите, чтобы рядом был сопровождающий?'),
('medicine','privacy','Осмотр и обследования
Пример: We will protect your privacy during the examination.','
Перевод: Мы обеспечим вашу конфиденциальность во время осмотра.'),
('medicine','inspection','Осмотр и обследования
Пример: Inspection is the first part of the examination.','
Перевод: Осмотр – первая часть обследования.'),
('medicine','palpation','Осмотр и обследования
Пример: Tell me if palpation causes discomfort.','
Перевод: Скажите, если пальпация вызывает дискомфорт.'),
('medicine','percussion','Осмотр и обследования
Пример: I will explain the percussion part of the examination.','
Перевод: Я объясню часть обследования, связанную с перкуссией.'),
('medicine','auscultation','Осмотр и обследования
Пример: Auscultation was performed with a stethoscope.','
Перевод: Аускультация проводилась со стетоскопом.'),
('medicine','stethoscope','Осмотр и обследования
Пример: The stethoscope may feel cold.','
Перевод: Стетоскоп может ощущаться холодным.'),
('medicine','blood test','Осмотр и обследования
Пример: I will explain why we are considering a blood test.','
Перевод: Я объясню, почему мы рассматриваем анализ крови.'),
('medicine','sample','Осмотр и обследования
Пример: Please label the sample correctly.','
Перевод: Пожалуйста, правильно подпишите образец.'),
('medicine','specimen','Осмотр и обследования
Пример: Check the specimen label before sending it.','
Перевод: Проверьте маркировку образца перед отправкой.'),
('medicine','full blood count','Осмотр и обследования
Пример: The full blood count is pending.','
Перевод: Результат общего анализа крови ещё не готов.'),
('medicine','complete blood count','Осмотр и обследования
Пример: The complete blood count has been reviewed.','
Перевод: Полный анализ крови рассмотрен.'),
('medicine','hemoglobin','Осмотр и обследования
Пример: Check the hemoglobin result.','
Перевод: Проверьте результат гемоглобина.'),
('medicine','platelet','Осмотр и обследования
Пример: The report includes the platelet count.','
Перевод: В отчёте указано количество тромбоцитов.'),
('medicine','white blood cell','Осмотр и обследования
Пример: The white blood cell count is recorded here.','
Перевод: Здесь записано количество лейкоцитов.'),
('medicine','creatinine','Осмотр и обследования
Пример: The creatinine result is available.','
Перевод: Результат креатинина доступен.'),
('medicine','electrolyte','Осмотр и обследования
Пример: The electrolyte results are pending.','
Перевод: Результаты анализа на электролиты ещё не готовы.'),
('medicine','reference range','Осмотр и обследования
Пример: Compare the result with the laboratory reference range.','
Перевод: Сравните результат с референсным диапазоном лаборатории.'),
('medicine','imaging','Осмотр и обследования
Пример: The imaging report is not yet available.','
Перевод: Отчёт по визуализации ещё недоступен.'),
('medicine','x-ray','Осмотр и обследования
Пример: Have you had an X-ray before?','
Перевод: У вас уже был рентген?'),
('medicine','ultrasound','Осмотр и обследования
Пример: I will explain what the ultrasound involves.','
Перевод: Я объясню, что включает ультразвуковое исследование.'),
('medicine','ct scan','Осмотр и обследования
Пример: The CT scan report has been reviewed.','
Перевод: Отчёт КТ рассмотрен.'),
('medicine','mri scan','Осмотр и обследования
Пример: Do you have any questions about the MRI scan?','
Перевод: Есть ли у вас вопросы по МРТ?'),
('medicine','biopsy','Осмотр и обследования
Пример: The clinician will explain the biopsy procedure.','
Перевод: Клинический специалист объяснит процедуру биопсии.'),
('medicine','culture','Осмотр и обследования
Пример: The sample has been sent for culture.','
Перевод: Образец отправлен на посев.'),
('medicine','pending','Осмотр и обследования
Пример: The blood test results are pending.','
Перевод: Результаты анализа крови в ожидании.'),
('medicine','finding','Осмотр и обследования
Пример: Explain the findings in plain language.','
Перевод: Объясните результаты простыми словами.'),
('medicine','incidental finding','Осмотр и обследования
Пример: The report mentions an incidental finding.','
Перевод: В отчёте упомянуто случайное обнаружение.'),
('medicine','diagnosis','Диагноз, план и документация
Пример: We need more information before confirming the diagnosis.','
Перевод: Нужна дополнительная информация для подтверждения диагноза.'),
('medicine','differential diagnosis','Диагноз, план и документация
Пример: Discuss the differential diagnosis with the team.','
Перевод: Обсудите дифференциальный диагноз с командой.'),
('medicine','provisional diagnosis','Диагноз, план и документация
Пример: Record the provisional diagnosis clearly.','
Перевод: Запишите предварительный диагноз чётко.'),
('medicine','assessment','Диагноз, план и документация
Пример: The assessment is documented in the notes.','
Перевод: Оценка задокументирована в примечаниях.'),
('medicine','management plan','Диагноз, план и документация
Пример: Let us discuss the management plan together.','
Перевод: Давайте обсудим план лечения вместе.'),
('medicine','prognosis','Диагноз, план и документация
Пример: The patient has questions about the prognosis.','
Перевод: У пациента есть вопросы о прогнозе.'),
('medicine','follow-up','Диагноз, план и документация
Пример: How will the follow-up be arranged?','
Перевод: Как будет организовано последующее наблюдение?'),
('medicine','review','Диагноз, план и документация
Пример: We will arrange a review of the results.','
Перевод: Мы организуем повторный просмотр результатов.'),
('medicine','monitoring','Диагноз, план и документация
Пример: Explain the purpose of monitoring.','
Перевод: Объясните цель мониторинга.'),
('medicine','observation','Диагноз, план и документация
Пример: Record the observations in the chart.','
Перевод: Запишите результаты наблюдения в медицинскую карте.'),
('medicine','admission','Диагноз, план и документация
Пример: The admission note is complete.','
Перевод: Записка о госпитализации завершена.'),
('medicine','discharge','Диагноз, план и документация
Пример: The discharge summary is ready.','
Перевод: Выписка готова.'),
('medicine','discharge summary','Диагноз, план и документация
Пример: Send the discharge summary to the general practitioner.','
Перевод: Отправьте выписной эпикриз врачу общей практики.'),
('medicine','inpatient','Диагноз, план и документация
Пример: The inpatient team will review the case.','
Перевод: Команда стационара рассмотрит этот клинический случай.'),
('medicine','outpatient','Диагноз, план и документация
Пример: The outpatient appointment is next week.','
Перевод: Амбулаторный приём запланирован на следующую неделю.'),
('medicine','ward','Диагноз, план и документация
Пример: Which ward is the patient on?','
Перевод: В какой палате находится пациент?'),
('medicine','emergency department','Диагноз, план и документация
Пример: The patient was seen in the emergency department.','
Перевод: Пациент был осмотрен в отделении неотложной помощи.'),
('medicine','intensive care unit','Диагноз, план и документация
Пример: The handover is for the intensive care unit.','
Перевод: Передача информации для отделения интенсивной терапии.'),
('medicine','handover','Диагноз, план и документация
Пример: Give a structured handover to the next team.','
Перевод: Сделайте структурированную передачу следующей команде.'),
('medicine','medical record','Диагноз, план и документация
Пример: Please update the medical record.','
Перевод: Пожалуйста, обновите медицинскую карту.'),
('medicine','chart','Диагноз, план и документация
Пример: Check the medication chart.','
Перевод: Проверьте медицинскую карту.'),
('medicine','clinical note','Диагноз, план и документация
Пример: Write a clear clinical note.','
Перевод: Составьте чёткую клиническую запись.'),
('medicine','confidentiality','Диагноз, план и документация
Пример: Patient confidentiality must be respected.','
Перевод: Необходимо соблюдать конфиденциальность пациента.'),
('medicine','capacity','Диагноз, план и документация
Пример: Document the assessment of decision-making capacity.','
Перевод: Задокументируйте оценку дееспособности для принятия решений.'),
('medicine','shared decision-making','Диагноз, план и документация
Пример: Shared decision-making includes the patient''s preferences.','
Перевод: Совместное принятие решений учитывает предпочтения пациента.'),
('medicine','safety-netting','Диагноз, план и документация
Пример: Document the safety-netting discussion.','
Перевод: Задокументируйте обсуждение мер предосторожности (safety‑netting).'),
('medicine','red flag','Диагноз, план и документация
Пример: Discuss any red flags with the supervising clinician.','
Перевод: Обсудите любые тревожные сигналы с руководящим врачом.'),
('medicine','escalation','Диагноз, план и документация
Пример: Follow the local escalation procedure.','
Перевод: Следуйте местной процедуре эскалации.'),
('medicine','clinical guideline','Диагноз, план и документация
Пример: Consult the relevant clinical guideline.','
Перевод: Обратитесь к соответствующей клинической рекомендации.'),
('medicine','evidence-based','Диагноз, план и документация
Пример: The team discussed evidence-based practice.','
Перевод: Команда обсудила практику, основанную на доказательствах.'),
('medicine','plain language','Профессиональное общение с пациентом
Пример: Explain the result in plain language.','
Перевод: Объясните результат простыми словами.'),
('medicine','empathy','Профессиональное общение с пациентом
Пример: Show empathy when discussing the patient''s concerns.','
Перевод: Проявляйте эмпатию, обсуждая беспокойства пациента.'),
('medicine','reassurance','Профессиональное общение с пациентом
Пример: Offer reassurance without dismissing concerns.','
Перевод: Успокойте, не отбрасывая его опасения.'),
('medicine','expectation','Профессиональное общение с пациентом
Пример: What are your expectations for this consultation?','
Перевод: Каковы ваши ожидания от этой консультации?'),
('medicine','preference','Профессиональное общение с пациентом
Пример: What is your preference?','
Перевод: Какой вариант вы предпочитаете?'),
('medicine','understanding','Профессиональное общение с пациентом
Пример: I would like to check your understanding.','
Перевод: Я хотел(а) бы проверить ваше понимание.'),
('medicine','teach-back','Профессиональное общение с пациентом
Пример: Use teach-back to check that the explanation was clear.','
Перевод: Используйте метод «teach‑back», чтобы убедиться, что объяснение было понятно.'),
('medicine','interpreter','Профессиональное общение с пациентом
Пример: Would you like a professional interpreter?','
Перевод: Хотите ли вы профессионального переводчика?'),
('medicine','next of kin','Профессиональное общение с пациентом
Пример: Who is listed as your next of kin?','
Перевод: Кто указан у вас в качестве ближайшего родственника?'),
('medicine','caregiver','Профессиональное общение с пациентом
Пример: Does your caregiver help with daily activities?','
Перевод: Помогает ли ваш ухаживающий человек в повседневных делах?'),
('medicine','carer','Профессиональное общение с пациентом
Пример: Would you like your carer to join us?','
Перевод: Хотите, чтобы ваш ухаживающий присоединился к нам?'),
('medicine','sensitive question','Профессиональное общение с пациентом
Пример: May I ask a sensitive question?','
Перевод: Можно задать деликатный вопрос?'),
('medicine','informed consent','Профессиональное общение с пациентом
Пример: Allow time for questions before seeking informed consent.','
Перевод: Выделите время для вопросов перед получением информированного согласия.'),
('medicine','benefit','Профессиональное общение с пациентом
Пример: Let us discuss the potential benefits.','
Перевод: Давайте обсудим потенциальные преимущества.'),
('medicine','risk','Профессиональное общение с пациентом
Пример: What would you like to know about the risks?','
Перевод: Что бы вы хотели узнать о рисках?'),
('medicine','alternative','Профессиональное общение с пациентом
Пример: We can discuss the available alternatives.','
Перевод: Мы можем обсудить доступные альтернативы.'),
('medicine','uncertainty','Профессиональное общение с пациентом
Пример: Explain the uncertainty honestly.','
Перевод: Честно объясните степень неопределённости.'),
('medicine','what brings you in today?','Профессиональное общение с пациентом
Пример: What brings you in today?','
Перевод: Что привело вас к нам сегодня?'),
('medicine','could you tell me more?','Профессиональное общение с пациентом
Пример: Could you tell me more about the pain?','
Перевод: Можете рассказать подробнее о боли?'),
('medicine','when did it start?','Профессиональное общение с пациентом
Пример: When did it start?','
Перевод: Когда она началась?'),
('medicine','what makes it worse?','Профессиональное общение с пациентом
Пример: What makes it worse?','
Перевод: Что усиливает её?'),
('medicine','what helps relieve it?','Профессиональное общение с пациентом
Пример: What helps relieve it?','
Перевод: Что помогает её облегчить?'),
('medicine','how is this affecting your daily life?','Профессиональное общение с пациентом
Пример: How is this affecting your daily life?','
Перевод: Как это влияет на вашу повседневную жизнь?'),
('medicine','i can see this is worrying you.','Профессиональное общение с пациентом
Пример: I can see this is worrying you.','
Перевод: Вижу, это вас тревожит.'),
('medicine','may i examine you?','Профессиональное общение с пациентом
Пример: May I examine you?','
Перевод: Можно вас осмотреть?'),
('medicine','please let me know if it hurts.','Профессиональное общение с пациентом
Пример: Please let me know if it hurts.','
Перевод: Сообщите, если будет больно.'),
('medicine','what questions do you have?','Профессиональное общение с пациентом
Пример: What questions do you have?','
Перевод: Какие у вас вопросы?'),
('medicine','let me check i have understood.','Профессиональное общение с пациентом
Пример: Let me check I have understood.','
Перевод: Позвольте убедиться, что я правильно понял(а).'),
('medicine','could you explain it back in your own words?','Профессиональное общение с пациентом
Пример: Could you explain it back in your own words?','
Перевод: Можете пересказать своими словами?'),
('medicine','we will decide together.','Профессиональное общение с пациентом
Пример: We will decide together.','
Перевод: Мы примем решение вместе.'),
('medicine','anatomy','Общая анатомия и неврология
Пример: The course covers basic anatomy.','
Перевод: Курс охватывает базовую анатомию.'),
('medicine','tissue','Общая анатомия и неврология
Пример: The sample contains soft tissue.','
Перевод: Образец содержит мягкие ткани.'),
('medicine','organ','Общая анатомия и неврология
Пример: The report describes the affected organ.','
Перевод: В отчёте описывается поражённый орган.'),
('medicine','bone','Общая анатомия и неврология
Пример: Can you point to the painful area over the bone?','
Перевод: Укажите, пожалуйста, болезненную область над костью.'),
('medicine','joint','Общая анатомия и неврология
Пример: Which joint is painful?','
Перевод: Какой сустав болит?'),
('medicine','muscle','Общая анатомия и неврология
Пример: Does the muscle feel weak?','
Перевод: Чувствуется ли слабость мышцы?'),
('medicine','tendon','Общая анатомия и неврология
Пример: The examination includes the tendon.','
Перевод: Осмотр включает сухожилие.'),
('medicine','ligament','Общая анатомия и неврология
Пример: The imaging report describes the ligament.','
Перевод: В рентгенологическом отчёте описана связка.'),
('medicine','nerve','Общая анатомия и неврология
Пример: The symptoms may follow a nerve distribution.','
Перевод: Симптомы могут следовать по нервному распределению.'),
('medicine','artery','Общая анатомия и неврология
Пример: The report describes the artery.','
Перевод: В отчёте описана артерия.'),
('medicine','vein','Общая анатомия и неврология
Пример: The sample is taken from a vein.','
Перевод: Образец взят из вены.'),
('medicine','spine','Общая анатомия и неврология
Пример: Have you had surgery on your spine?','
Перевод: Была ли у вас операция на позвоночнике?'),
('medicine','spinal cord','Общая анатомия и неврология
Пример: The report describes the spinal cord.','
Перевод: В отчёте описан спинной мозг.'),
('medicine','brain','Общая анатомия и неврология
Пример: The imaging report includes the brain.','
Перевод: В рентгенологическом отчёте включён мозг.'),
('medicine','consciousness','Общая анатомия и неврология
Пример: Was there any loss of consciousness?','
Перевод: Было ли потеря сознания?'),
('medicine','alert','Общая анатомия и неврология
Пример: The patient is alert and able to answer questions.','
Перевод: Пациент в ясном сознании и может отвечать на вопросы.'),
('medicine','orientation','Общая анатомия и неврология
Пример: Assess orientation during the examination.','
Перевод: Оцените ориентацию во время осмотра.'),
('medicine','confusion','Общая анатомия и неврология
Пример: When did the confusion begin?','
Перевод: Когда появилась спутанность сознания?'),
('medicine','seizure','Общая анатомия и неврология
Пример: Was the seizure witnessed?','
Перевод: Был ли эпилептический приступ наблюдаем?'),
('medicine','tremor','Общая анатомия и неврология
Пример: When do you notice the tremor?','
Перевод: Когда вы замечаете дрожь?'),
('medicine','gait','Общая анатомия и неврология
Пример: The examination includes gait assessment.','
Перевод: Осмотр включает оценку походки.'),
('medicine','reflex','Общая анатомия и неврология
Пример: I am going to check your reflexes.','
Перевод: Я проверю ваши рефлексы.'),
('medicine','sensation','Общая анатомия и неврология
Пример: Tell me whether the sensation feels the same on both sides.','
Перевод: Скажите, одинаково ли ощущение с обеих сторон.'),
('medicine','range of motion','Общая анатомия и неврология
Пример: We will assess the range of motion.','
Перевод: Мы оценим диапазон движений.'),
('medicine','fracture','Общая анатомия и неврология
Пример: The report describes a fracture.','
Перевод: В отчёте описан перелом.'),
('medicine','sprain','Общая анатомия и неврология
Пример: The referral mentions an ankle sprain.','
Перевод: В направлении указано растяжение связок голеностопного сустава.'),
('medicine','strain','Общая анатомия и неврология
Пример: The notes describe a muscle strain.','
Перевод: В записях описано растяжение мышцы.'),
('medicine','concussion','Общая анатомия и неврология
Пример: The clinician assessed the patient for concussion.','
Перевод: Врач оценил пациента на предмет сотрясения мозга.'),
('medicine','loss of consciousness','Общая анатомия и неврология
Пример: Was there any loss of consciousness?','
Перевод: Было ли потеря сознания?'),
('medicine','functional limitation','Общая анатомия и неврология
Пример: Describe any functional limitations.','
Перевод: Опишите любые функциональные ограничения.'),
('medicine','infection','Инфекции, уход и дополнительные области
Пример: Have you recently had an infection?','
Перевод: Были ли у вас недавно инфекции?'),
('medicine','inflammation','Инфекции, уход и дополнительные области
Пример: The report describes signs of inflammation.','
Перевод: В отчёте описаны признаки воспаления.'),
('medicine','contagious','Инфекции, уход и дополнительные области
Пример: The patient asks whether the condition is contagious.','
Перевод: Пациент спрашивает, заразно ли состояние.'),
('medicine','sterile','Инфекции, уход и дополнительные области
Пример: Check that the equipment is sterile.','
Перевод: Проверьте, стерильно ли оборудование.'),
('medicine','aseptic','Инфекции, уход и дополнительные области
Пример: Follow the local aseptic technique protocol.','
Перевод: Следуйте местному протоколу асептической техники.'),
('medicine','wound','Инфекции, уход и дополнительные области
Пример: When did the wound occur?','
Перевод: Когда произошла рана?'),
('medicine','dressing','Инфекции, уход и дополнительные области
Пример: When was the dressing last changed?','
Перевод: Когда в последний раз менялась повязка?'),
('medicine','suture','Инфекции, уход и дополнительные области
Пример: The note records the number of sutures.','
Перевод: В записи указано количество швов.'),
('medicine','healing','Инфекции, уход и дополнительные области
Пример: The appointment is to review wound healing.','
Перевод: Приём назначен для контроля за заживлением раны.'),
('medicine','vaccination','Инфекции, уход и дополнительные области
Пример: Can we review your vaccination history?','
Перевод: Можем ли мы просмотреть вашу историю вакцинаций?'),
('medicine','immunization','Инфекции, уход и дополнительные области
Пример: The immunization record is available.','
Перевод: Запись о прививках доступна.'),
('medicine','pregnancy','Инфекции, уход и дополнительные области
Пример: Is there any possibility of pregnancy?','
Перевод: Есть ли вероятность беременности?'),
('medicine','menstrual cycle','Инфекции, уход и дополнительные области
Пример: Has your menstrual cycle changed?','
Перевод: Изменился ли ваш менструальный цикл?'),
('medicine','last menstrual period','Инфекции, уход и дополнительные области
Пример: When was your last menstrual period?','
Перевод: Когда был ваш последний менструальный период?'),
('medicine','breastfeeding','Инфекции, уход и дополнительные области
Пример: Are you currently breastfeeding?','
Перевод: Вы сейчас кормите грудью?'),
('medicine','antenatal','Инфекции, уход и дополнительные области
Пример: The antenatal appointment has been arranged.','
Перевод: Назначена предродовая консультация.'),
('medicine','prenatal','Инфекции, уход и дополнительные области
Пример: The prenatal record has been reviewed.','
Перевод: Пройдена проверка пренатальной карты.'),
('medicine','pediatric','Инфекции, уход и дополнительные области
Пример: The pediatric team will review the referral.','
Перевод: Педиатрическая команда рассмотрит направление.'),
('medicine','developmental milestone','Инфекции, уход и дополнительные области
Пример: Ask about developmental milestones.','
Перевод: Спросите о достижении возрастных вех развития.'),
('medicine','mental health','Инфекции, уход и дополнительные области
Пример: Would you like to discuss your mental health?','
Перевод: Хотите обсудить ваше психическое здоровье?'),
('medicine','anxiety','Инфекции, уход и дополнительные области
Пример: How has anxiety affected your sleep?','
Перевод: Как тревога влияет на ваш сон?'),
('medicine','depression','Инфекции, уход и дополнительные области
Пример: Have you previously received care for depression?','
Перевод: Получали ли вы ранее лечение от депрессии?'),
('medicine','sleep disturbance','Инфекции, уход и дополнительные области
Пример: When did the sleep disturbance begin?','
Перевод: Когда начались нарушения сна?'),
('medicine','screening','Инфекции, уход и дополнительные области
Пример: I will explain the purpose of screening.','
Перевод: Я объясню цель скрининга.'),
('medicine','rehabilitation','Инфекции, уход и дополнительные области
Пример: The team will discuss rehabilitation goals.','
Перевод: Команда обсудит цели реабилитации.'),
('medicine','physiotherapy','Инфекции, уход и дополнительные области
Пример: The referral is for physiotherapy.','
Перевод: Направление на физиотерапию.'),
('medicine','occupational therapy','Инфекции, уход и дополнительные области
Пример: Occupational therapy focuses on everyday activities.','
Перевод: Эрготерапия ориентирована на повседневные действия.'),
('medicine','palliative care','Инфекции, уход и дополнительные области
Пример: The team discussed palliative care needs.','
Перевод: Команда обсудила потребности паллиативного ухода.'),
('medicine','advance care planning','Инфекции, уход и дополнительные области
Пример: The patient would like to discuss advance care planning.','
Перевод: Пациент хочет обсудить планирование завещания о лечении.'),
('medicine','quality of life','Инфекции, уход и дополнительные области
Пример: What matters most to your quality of life?','
Перевод: Что для вас наиболее важно в качестве жизни?'),
('it','developer','Работа в IT и команда
Пример: I work as a software developer.','
Перевод: Я работаю разработчиком программного обеспечения.'),
('it','engineer','Работа в IT и команда
Пример: The engineer investigated the issue.','
Перевод: Инженер исследовал проблему.'),
('it','software','Работа в IT и команда
Пример: We develop software for small businesses.','
Перевод: Мы разрабатываем программное обеспечение для малого бизнеса.'),
('it','hardware','Работа в IT и команда
Пример: This looks like a hardware problem.','
Перевод: Это выглядит как аппаратная проблема.'),
('it','application','Работа в IT и команда
Пример: The application is running slowly.','
Перевод: Приложение работает медленно.'),
('it','service','Работа в IT и команда
Пример: The service is temporarily unavailable.','
Перевод: Сервис временно недоступен.'),
('it','platform','Работа в IT и команда
Пример: Which platform does this application support?','
Перевод: Какие платформы поддерживает это приложение?'),
('it','product','Работа в IT и команда
Пример: What problem does the product solve?','
Перевод: Какую проблему решает продукт?'),
('it','feature','Работа в IT и команда
Пример: We are working on a new feature.','
Перевод: Мы работаем над новой функцией.'),
('it','requirement','Работа в IT и команда
Пример: Could you clarify this requirement?','
Перевод: Не могли бы вы уточнить это требование?'),
('it','specification','Работа в IT и команда
Пример: The specification needs updating.','
Перевод: Спецификация требует обновления.'),
('it','stakeholder','Работа в IT и команда
Пример: We need feedback from the stakeholders.','
Перевод: Нам нужен отзыв от заинтересованных сторон.'),
('it','user','Работа в IT и команда
Пример: How will the user complete this task?','
Перевод: Как пользователь выполнит эту задачу?'),
('it','customer','Работа в IT и команда
Пример: The customer reported a problem.','
Перевод: Клиент сообщил о проблеме.'),
('it','team','Работа в IT и команда
Пример: I will discuss it with the team.','
Перевод: Я обсудю это с командой.'),
('it','colleague','Работа в IT и команда
Пример: My colleague reviewed the change.','
Перевод: Мой коллега проверил изменение.'),
('it','manager','Работа в IT и команда
Пример: I will check with my manager.','
Перевод: Я уточню у своего менеджера.'),
('it','meeting','Работа в IT и команда
Пример: Can we schedule a short meeting?','
Перевод: Можем ли мы запланировать короткую встречу?'),
('it','stand-up','Работа в IT и команда
Пример: I will mention the blocker at stand-up.','
Перевод: На стендапе я расскажу о том, что мешает продолжить работу.'),
('it','sprint','Работа в IT и команда
Пример: This task is planned for the next sprint.','
Перевод: Эта задача запланирована на следующий спринт.'),
('it','backlog','Работа в IT и команда
Пример: Add the request to the backlog.','
Перевод: Добавьте запрос в бэклог.'),
('it','ticket','Работа в IT и команда
Пример: Please create a ticket for this issue.','
Перевод: Пожалуйста, создайте тикет для этой проблемы.'),
('it','task','Работа в IT и команда
Пример: The task is ready for review.','
Перевод: Задача готова к ревью.'),
('it','priority','Работа в IT и команда
Пример: What is the priority of this issue?','
Перевод: Каков приоритет этой проблемы?'),
('it','deadline','Работа в IT и команда
Пример: Can we agree on a realistic deadline?','
Перевод: Можем ли мы согласовать реалистичный срок?'),
('it','estimate','Работа в IT и команда
Пример: I need more details before giving an estimate.','
Перевод: Мне нужно больше деталей перед оценкой.'),
('it','scope','Работа в IT и команда
Пример: This request changes the scope.','
Перевод: Этот запрос меняет объём работ.'),
('it','blocker','Работа в IT и команда
Пример: I have a blocker with the test environment.','
Перевод: У меня блокер с тестовой средой.'),
('it','progress','Работа в IT и команда
Пример: Here is a quick progress update.','
Перевод: Вот краткое обновление прогресса.'),
('it','feedback','Работа в IT и команда
Пример: Thank you for the feedback.','
Перевод: Спасибо за отзыв.'),
('it','code','Код и базовые понятия
Пример: The code is ready for review.','
Перевод: Код готов к ревью.'),
('it','source code','Код и базовые понятия
Пример: Where is the source code stored?','
Перевод: Где хранится исходный код?'),
('it','function','Код и базовые понятия
Пример: This function returns a value.','
Перевод: Эта функция возвращает значение.'),
('it','method','Код и базовые понятия
Пример: This method updates the object.','
Перевод: Этот метод обновляет объект.'),
('it','variable','Код и базовые понятия
Пример: Use a clear variable name.','
Перевод: Используйте понятное имя переменной.'),
('it','constant','Код и базовые понятия
Пример: This value should be a constant.','
Перевод: Это значение должно быть константой.'),
('it','type','Код и базовые понятия
Пример: What type does this function return?','
Перевод: Какой тип возвращает эта функция?'),
('it','string','Код и базовые понятия
Пример: The input is an empty string.','
Перевод: Ввод — пустая строка.'),
('it','integer','Код и базовые понятия
Пример: The parameter must be an integer.','
Перевод: Параметр должен быть целым числом.'),
('it','boolean','Код и базовые понятия
Пример: The function returns a boolean.','
Перевод: Функция возвращает логическое значение.'),
('it','array','Код и базовые понятия
Пример: The array contains ten elements.','
Перевод: Массив содержит десять элементов.'),
('it','list','Код и базовые понятия
Пример: Add the item to the list.','
Перевод: Добавьте элемент в список.'),
('it','map','Код и базовые понятия
Пример: Store the values in a map.','
Перевод: Сохраните значения в карте.'),
('it','set','Код и базовые понятия
Пример: The set contains unique values.','
Перевод: Множество содержит уникальные значения.'),
('it','object','Код и базовые понятия
Пример: Create a new object.','
Перевод: Создайте новый объект.'),
('it','class','Код и базовые понятия
Пример: This class handles file access.','
Перевод: Этот класс обрабатывает доступ к файлам.'),
('it','interface','Код и базовые понятия
Пример: The interface should remain stable.','
Перевод: Интерфейс должен оставаться стабильным.'),
('it','module','Код и базовые понятия
Пример: The module has no external dependencies.','
Перевод: Модуль не имеет внешних зависимостей.'),
('it','library','Код и базовые понятия
Пример: We use a standard library function.','
Перевод: Мы используем функцию стандартной библиотеки.'),
('it','framework','Код и базовые понятия
Пример: Which framework does the project use?','
Перевод: Какой фреймворк используется в проекте?'),
('it','package','Код и базовые понятия
Пример: Install the required package.','
Перевод: Установите требуемый пакет.'),
('it','dependency','Код и базовые понятия
Пример: We need to update this dependency.','
Перевод: Нужно обновить эту зависимость.'),
('it','parameter','Код и базовые понятия
Пример: This parameter is optional.','
Перевод: Этот параметр необязателен.'),
('it','argument','Код и базовые понятия
Пример: Pass the file name as an argument.','
Перевод: Передайте имя файла в качестве аргумента.'),
('it','return value','Код и базовые понятия
Пример: Check the return value.','
Перевод: Проверьте возвращаемое значение.'),
('it','condition','Код и базовые понятия
Пример: The condition is always false.','
Перевод: Условие всегда ложно.'),
('it','loop','Код и базовые понятия
Пример: The loop processes each item.','
Перевод: Цикл обрабатывает каждый элемент.'),
('it','iteration','Код и базовые понятия
Пример: The error occurs on the second iteration.','
Перевод: Ошибка возникает на второй итерации.'),
('it','branch','Код и базовые понятия
Пример: Create a branch for this change.','
Перевод: Создайте ветку для этого изменения.'),
('it','exception','Код и базовые понятия
Пример: The function throws an exception.','
Перевод: Функция бросает исключение.'),
('it','repository','Репозиторий и совместная работа
Пример: Clone the repository first.','
Перевод: Сначала клонируйте репозиторий.'),
('it','version control','Репозиторий и совместная работа
Пример: We use version control for all changes.','
Перевод: Мы используем систему контроля версий для всех изменений.'),
('it','commit','Репозиторий и совместная работа
Пример: Keep the commit message clear.','
Перевод: Сделайте сообщение коммита понятным.'),
('it','merge','Репозиторий и совместная работа
Пример: Merge the branch after review.','
Перевод: Слейте ветку после ревью.'),
('it','pull request','Репозиторий и совместная работа
Пример: I opened a pull request.','
Перевод: Я открыл запрос на слияние.'),
('it','review','Репозиторий и совместная работа
Пример: Could you review my change?','
Перевод: Не могли бы вы проверить моё изменение?'),
('it','approval','Репозиторий и совместная работа
Пример: The change needs approval before merging.','
Перевод: Изменение требует одобрения перед слиянием.'),
('it','conflict','Репозиторий и совместная работа
Пример: I resolved the merge conflict.','
Перевод: Я разрешил конфликт слияния.'),
('it','rebase','Репозиторий и совместная работа
Пример: Rebase the branch on the latest main.','
Перевод: Перебазируйте ветку на последнюю главную ветку.'),
('it','rollback','Репозиторий и совместная работа
Пример: We prepared a rollback plan.','
Перевод: Мы подготовили план отката.'),
('it','revert','Репозиторий и совместная работа
Пример: We need to revert this commit.','
Перевод: Нужно откатить этот коммит.'),
('it','patch','Репозиторий и совместная работа
Пример: The patch fixes the reported issue.','
Перевод: Патч исправляет сообщённую проблему.'),
('it','diff','Репозиторий и совместная работа
Пример: The diff contains a small change.','
Перевод: Diff содержит небольшое изменение.'),
('it','changelog','Репозиторий и совместная работа
Пример: Update the changelog before release.','
Перевод: Обновите журнал изменений перед релизом.'),
('it','tag','Репозиторий и совместная работа
Пример: Create a tag for the release.','
Перевод: Создайте тег для релиза.'),
('it','release','Репозиторий и совместная работа
Пример: The release is scheduled for Friday.','
Перевод: Релиз запланирован на пятницу.'),
('it','version','Репозиторий и совместная работа
Пример: Which version are you using?','
Перевод: Какую версию вы используете?'),
('it','upgrade','Репозиторий и совместная работа
Пример: Test the upgrade in staging.','
Перевод: Протестируйте обновление в стейджинге.'),
('it','compatibility','Репозиторий и совместная работа
Пример: We need to preserve compatibility.','
Перевод: Нужно сохранить совместимость.'),
('it','breaking change','Репозиторий и совместная работа
Пример: This is a breaking change.','
Перевод: Это критическое изменение.'),
('it','backward compatibility','Репозиторий и совместная работа
Пример: The update preserves backward compatibility.','
Перевод: Обновление сохраняет обратную совместимость.'),
('it','deprecation','Репозиторий и совместная работа
Пример: The documentation includes a deprecation notice.','
Перевод: В документации есть уведомление об устаревании.'),
('it','documentation','Репозиторий и совместная работа
Пример: The documentation explains the setup.','
Перевод: В документации объяснена настройка.'),
('it','readme','Репозиторий и совместная работа
Пример: Follow the instructions in the README.','
Перевод: Следуйте инструкциям в README.'),
('it','comment','Репозиторий и совместная работа
Пример: This comment explains the reason for the check.','
Перевод: Этот комментарий объясняет причину проверки.'),
('it','convention','Репозиторий и совместная работа
Пример: Follow the project''s naming conventions.','
Перевод: Следуйте принятым в проекте правилам именования.'),
('it','refactor','Репозиторий и совместная работа
Пример: I want to refactor this function.','
Перевод: Я хочу рефакторить эту функцию.'),
('it','cleanup','Репозиторий и совместная работа
Пример: This is a small code cleanup.','
Перевод: Это небольшая очистка кода.'),
('it','maintainer','Репозиторий и совместная работа
Пример: Ask the maintainer before changing the public interface.','
Перевод: Спросите у сопровождающего перед изменением публичного интерфейса.'),
('it','contributor','Репозиторий и совместная работа
Пример: The contributor added a useful test.','
Перевод: Участник добавил полезный тест.'),
('it','bug','Ошибки и тестирование
Пример: I found a bug in the login flow.','
Перевод: Я нашёл ошибку в процессе входа в систему.'),
('it','defect','Ошибки и тестирование
Пример: The defect affects older versions.','
Перевод: Дефект затрагивает более старые версии.'),
('it','issue','Ошибки и тестирование
Пример: Can you reproduce the issue?','
Перевод: Можете воспроизвести проблему?'),
('it','error','Ошибки и тестирование
Пример: What error message do you see?','
Перевод: Какое сообщение об ошибке вы видите?'),
('it','failure','Ошибки и тестирование
Пример: The test failure is reproducible.','
Перевод: Сбой теста воспроизводим.'),
('it','crash','Ошибки и тестирование
Пример: The application crashes on startup.','
Перевод: Приложение падает при запуске.'),
('it','hang','Ошибки и тестирование
Пример: The process hangs after the request.','
Перевод: Процесс зависает после запроса.'),
('it','freeze','Ошибки и тестирование
Пример: The screen freezes when I click this.','
Перевод: Экран зависает, когда я нажимаю сюда.'),
('it','reproduce','Ошибки и тестирование
Пример: I cannot reproduce it locally.','
Перевод: Мне не удаётся воспроизвести это локально.'),
('it','reproduction steps','Ошибки и тестирование
Пример: Please include the reproduction steps.','
Перевод: Пожалуйста, укажите шаги воспроизведения.'),
('it','expected behavior','Ошибки и тестирование
Пример: Describe the expected behavior.','
Перевод: Опишите ожидаемое поведение.'),
('it','actual behavior','Ошибки и тестирование
Пример: What is the actual behavior?','
Перевод: Каково фактическое поведение?'),
('it','root cause','Ошибки и тестирование
Пример: We found the root cause.','
Перевод: Мы нашли коренную причину.'),
('it','workaround','Ошибки и тестирование
Пример: There is a temporary workaround.','
Перевод: Есть временное обходное решение.'),
('it','fix','Ошибки и тестирование
Пример: The fix is included in this release.','
Перевод: Исправление включено в этот релиз.'),
('it','regression','Ошибки и тестирование
Пример: We added a test for the regression.','
Перевод: Мы добавили тест для регрессии.'),
('it','test case','Ошибки и тестирование
Пример: Add a test case for an empty input.','
Перевод: Добавьте тестовый случай для пустого ввода.'),
('it','unit test','Ошибки и тестирование
Пример: The unit test checks the calculation.','
Перевод: Юнит‑тест проверяет вычисление.'),
('it','integration test','Ошибки и тестирование
Пример: The integration test uses a temporary database.','
Перевод: Интеграционный тест использует временную базу данных.'),
('it','end-to-end test','Ошибки и тестирование
Пример: The end-to-end test covers the checkout flow.','
Перевод: End‑to‑end тест покрывает процесс оформления заказа.'),
('it','assertion','Ошибки и тестирование
Пример: The assertion failed.','
Перевод: Утверждение не выполнено.'),
('it','fixture','Ошибки и тестирование
Пример: The fixture creates a test user.','
Перевод: Фикстура создаёт тестового пользователя.'),
('it','mock','Ошибки и тестирование
Пример: Use a mock for the external service.','
Перевод: Используйте mock для внешнего сервиса.'),
('it','coverage','Ошибки и тестирование
Пример: Coverage alone does not prove correctness.','
Перевод: Один лишь охват кода не доказывает правильность.'),
('it','edge case','Ошибки и тестирование
Пример: We missed an edge case.','
Перевод: Мы упустили граничный случай.'),
('it','boundary','Ошибки и тестирование
Пример: Test the boundary values.','
Перевод: Протестируйте граничные значения.'),
('it','validation','Ошибки и тестирование
Пример: Input validation happens before saving.','
Перевод: Валидация ввода происходит перед сохранением.'),
('it','debug','Ошибки и тестирование
Пример: I need to debug this request.','
Перевод: Мне нужно отладить этот запрос.'),
('it','breakpoint','Ошибки и тестирование
Пример: Set a breakpoint before the call.','
Перевод: Установите точку останова перед вызовом.'),
('it','stack trace','Ошибки и тестирование
Пример: Can you share the stack trace?','
Перевод: Можете поделиться трассировкой стека?'),
('it','build','Сборка, процессы и память
Пример: The build failed.','
Перевод: Сборка не удалась.'),
('it','compile','Сборка, процессы и память
Пример: The code does not compile.','
Перевод: Код не компилируется.'),
('it','compiler','Сборка, процессы и память
Пример: The compiler reported a warning.','
Перевод: Компилятор выдал предупреждение.'),
('it','linker','Сборка, процессы и память
Пример: The linker cannot find the symbol.','
Перевод: Компоновщик не может найти символ.'),
('it','warning','Сборка, процессы и память
Пример: Please fix the compiler warning.','
Перевод: Пожалуйста, исправьте предупреждение компилятора.'),
('it','binary','Сборка, процессы и память
Пример: Run the newly built binary.','
Перевод: Запустите только что построенный бинарный файл.'),
('it','executable','Сборка, процессы и память
Пример: Where is the executable installed?','
Перевод: Где установлен исполняемый файл?'),
('it','runtime','Сборка, процессы и память
Пример: The error occurs at runtime.','
Перевод: Ошибка возникает во время выполнения.'),
('it','process','Сборка, процессы и память
Пример: The process is still running.','
Перевод: Процесс всё ещё работает.'),
('it','thread','Сборка, процессы и память
Пример: This code runs on another thread.','
Перевод: Этот код выполняется в другом потоке.'),
('it','concurrency','Сборка, процессы и память
Пример: The bug only appears under concurrency.','
Перевод: Ошибка проявляется только при параллельном выполнении.'),
('it','parallelism','Сборка, процессы и память
Пример: Parallelism can reduce processing time.','
Перевод: Параллелизм может сократить время обработки.'),
('it','synchronization','Сборка, процессы и память
Пример: This shared state needs synchronization.','
Перевод: Это общее состояние требует синхронизации.'),
('it','mutex','Сборка, процессы и память
Пример: The mutex protects the shared data.','
Перевод: Мьютекс защищает общие данные.'),
('it','lock','Сборка, процессы и память
Пример: Release the lock before the network call.','
Перевод: Освободите блокировку перед сетевым вызовом.'),
('it','deadlock','Сборка, процессы и память
Пример: The threads are stuck in a deadlock.','
Перевод: Потоки оказались во взаимной блокировке.'),
('it','race condition','Сборка, процессы и память
Пример: There is a race condition in this code.','
Перевод: В этом коде есть состояние гонки.'),
('it','memory','Сборка, процессы и память
Пример: The process uses too much memory.','
Перевод: Процесс использует слишком много памяти.'),
('it','allocation','Сборка, процессы и память
Пример: This operation performs a memory allocation.','
Перевод: Эта операция выполняет выделение памяти.'),
('it','leak','Сборка, процессы и память
Пример: We found a memory leak.','
Перевод: Мы обнаружили утечку памяти.'),
('it','stack','Сборка, процессы и память
Пример: This object is stored on the stack.','
Перевод: Этот объект хранится в стеке.'),
('it','heap','Сборка, процессы и память
Пример: The data is allocated on the heap.','
Перевод: Данные выделяются в куче.'),
('it','pointer','Сборка, процессы и память
Пример: Check whether the pointer is null.','
Перевод: Проверьте, что указатель не равен null.'),
('it','reference','Сборка, процессы и память
Пример: The function accepts a reference.','
Перевод: Функция принимает ссылку.'),
('it','ownership','Сборка, процессы и память
Пример: Resource ownership should be explicit.','
Перевод: Владение ресурсом должно быть явным.'),
('it','lifetime','Сборка, процессы и память
Пример: The reference must not outlive the object''s lifetime.','
Перевод: Ссылка не должна превышать время жизни объекта.'),
('it','resource','Сборка, процессы и память
Пример: Release the resource after use.','
Перевод: Освободите ресурс после использования.'),
('it','buffer','Сборка, процессы и память
Пример: Check the buffer size.','
Перевод: Проверьте размер буфера.'),
('it','overflow','Сборка, процессы и память
Пример: The check prevents an integer overflow.','
Перевод: Проверка предотвращает переполнение целого числа.'),
('it','undefined behavior','Сборка, процессы и память
Пример: This code may cause undefined behavior.','
Перевод: Этот код может вызвать неопределённое поведение.'),
('it','network','Сети и веб
Пример: The network connection is unstable.','
Перевод: Сетевое соединение нестабильно.'),
('it','server','Сети и веб
Пример: The server is not responding.','
Перевод: Сервер не отвечает.'),
('it','client','Сети и веб
Пример: The client sends a request.','
Перевод: Клиент отправляет запрос.'),
('it','request','Сети и веб
Пример: The request timed out.','
Перевод: Время ожидания запроса истекло.'),
('it','response','Сети и веб
Пример: Check the response status.','
Перевод: Проверьте статус ответа.'),
('it','endpoint','Сети и веб
Пример: Which endpoint handles this request?','
Перевод: Какой конечный пункт обрабатывает этот запрос?'),
('it','api','Сети и веб
Пример: The API returns JSON.','
Перевод: API возвращает JSON.'),
('it','http','Сети и веб
Пример: The service uses HTTP.','
Перевод: Сервис использует HTTP.'),
('it','url','Сети и веб
Пример: Check the URL for typos.','
Перевод: Проверьте URL на опечатки.'),
('it','header','Сети и веб
Пример: The authorization header is missing.','
Перевод: Отсутствует заголовок авторизации.'),
('it','payload','Сети и веб
Пример: The payload contains the user name.','
Перевод: В полезной нагрузке содержится имя пользователя.'),
('it','status code','Сети и веб
Пример: What status code did the server return?','
Перевод: Какой код состояния вернул сервер?'),
('it','timeout','Сети и веб
Пример: Increase the timeout only if necessary.','
Перевод: Увеличивайте тайм‑аут только при необходимости.'),
('it','retry','Сети и веб
Пример: The client will retry the request.','
Перевод: Клиент повторит запрос.'),
('it','latency','Сети и веб
Пример: We need to reduce latency.','
Перевод: Нужно уменьшить задержку.'),
('it','bandwidth','Сети и веб
Пример: The upload uses a lot of bandwidth.','
Перевод: Загрузка использует много пропускной способности.'),
('it','throughput','Сети и веб
Пример: Measure throughput under load.','
Перевод: Измерьте пропускную способность под нагрузкой.'),
('it','connection','Сети и веб
Пример: The connection was closed.','
Перевод: Соединение было закрыто.'),
('it','socket','Сети и веб
Пример: The socket is ready for reading.','
Перевод: Сокет готов к чтению.'),
('it','port','Сети и веб
Пример: Which port does the service use?','
Перевод: Какой порт использует сервис?'),
('it','protocol','Сети и веб
Пример: Both systems use the same protocol.','
Перевод: Обе системы используют один и тот же протокол.'),
('it','dns','Сети и веб
Пример: Check the DNS configuration.','
Перевод: Проверьте конфигурацию DNS.'),
('it','domain','Сети и веб
Пример: The domain name has changed.','
Перевод: Имя домена изменилось.'),
('it','proxy','Сети и веб
Пример: The request passes through a proxy.','
Перевод: Запрос проходит через прокси.'),
('it','firewall','Сети и веб
Пример: The firewall blocks this connection.','
Перевод: Брандмауэр блокирует это соединение.'),
('it','tls','Сети и веб
Пример: The connection uses TLS.','
Перевод: Соединение использует TLS.'),
('it','certificate','Сети и веб
Пример: The certificate has expired.','
Перевод: Сертификат истёк.'),
('it','webhook','Сети и веб
Пример: The payment service sends a webhook.','
Перевод: Сервис оплаты отправляет веб‑хук.'),
('it','cache','Сети и веб
Пример: The response was served from the cache.','
Перевод: Ответ был получен из кэша.'),
('it','cookie','Сети и веб
Пример: The session cookie has expired.','
Перевод: Сессионный cookie истёк.'),
('it','database','Базы данных и данные
Пример: The database is unavailable.','
Перевод: База данных недоступна.'),
('it','table','Базы данных и данные
Пример: The table stores user accounts.','
Перевод: Таблица хранит учётные записи пользователей.'),
('it','row','Базы данных и данные
Пример: The query returned one row.','
Перевод: Запрос вернул одну строку.'),
('it','column','Базы данных и данные
Пример: Add a column for the timestamp.','
Перевод: Добавьте столбец для метки времени.'),
('it','schema','Базы данных и данные
Пример: The migration changes the schema.','
Перевод: Миграция изменяет схему.'),
('it','query','Базы данных и данные
Пример: This query is slow.','
Перевод: Этот запрос работает медленно.'),
('it','index','Базы данных и данные
Пример: The index improves lookup speed.','
Перевод: Индекс ускоряет поиск.'),
('it','primary key','Базы данных и данные
Пример: Each row has a primary key.','
Перевод: Каждая строка имеет первичный ключ.'),
('it','foreign key','Базы данных и данные
Пример: The foreign key references the users table.','
Перевод: Внешний ключ ссылается на таблицу users.'),
('it','constraint','Базы данных и данные
Пример: The insert violated a constraint.','
Перевод: Вставка нарушила ограничение.'),
('it','transaction','Базы данных и данные
Пример: Save both changes in one transaction.','
Перевод: Сохраните оба изменения в одной транзакции.'),
('it','migration','Базы данных и данные
Пример: Back up the database before the migration.','
Перевод: Создайте резервную копию базы данных перед миграцией.'),
('it','backup','Базы данных и данные
Пример: We have a verified backup.','
Перевод: У нас есть проверенная резервная копия.'),
('it','restore','Базы данных и данные
Пример: Restore the backup into a test database.','
Перевод: Восстановите резервную копию в тестовой базе данных.'),
('it','insert','Базы данных и данные
Пример: Insert a new row.','
Перевод: Вставьте новую строку.'),
('it','update','Базы данных и данные
Пример: Update the user''s settings.','
Перевод: Обновите настройки пользователя.'),
('it','delete','Базы данных и данные
Пример: Delete only the selected records.','
Перевод: Удалите только выбранные записи.'),
('it','record','Базы данных и данные
Пример: The record already exists.','
Перевод: Запись уже существует.'),
('it','duplicate','Базы данных и данные
Пример: The unique constraint prevents duplicates.','
Перевод: Уникальное ограничение предотвращает дублирование.'),
('it','null','Базы данных и данные
Пример: null is different from an empty string.','
Перевод: null отличается от пустой строки.'),
('it','join','Базы данных и данные
Пример: The query uses a join.','
Перевод: Запрос использует соединение (JOIN).'),
('it','filter','Базы данных и данные
Пример: Filter the results by date.','
Перевод: Отфильтруйте результаты по дате.'),
('it','sort','Базы данных и данные
Пример: Sort the rows by creation time.','
Перевод: Отсортируйте строки по времени создания.'),
('it','aggregate','Базы данных и данные
Пример: Aggregate the results by user.','
Перевод: Сгруппируйте результаты по пользователю.'),
('it','consistency','Базы данных и данные
Пример: The transaction preserves consistency.','
Перевод: Транзакция сохраняет согласованность данных.'),
('it','isolation','Базы данных и данные
Пример: Check the transaction isolation level.','
Перевод: Проверьте уровень изоляции транзакции.'),
('it','serialization','Базы данных и данные
Пример: Serialization converts the object into a message.','
Перевод: Сериализация преобразует объект в сообщение.'),
('it','encoding','Базы данных и данные
Пример: Use UTF-8 encoding.','
Перевод: Используйте кодировку UTF‑8.'),
('it','deploy','Развёртывание и эксплуатация
Пример: We will deploy after the checks pass.','
Перевод: Мы развернём приложение после успешного прохождения проверок.'),
('it','deployment','Развёртывание и эксплуатация
Пример: The deployment completed successfully.','
Перевод: Развёртывание завершилось успешно.'),
('it','environment','Развёртывание и эксплуатация
Пример: Which environment has the problem?','
Перевод: В какой среде возникла проблема?'),
('it','production','Развёртывание и эксплуатация
Пример: Do not use test credentials in production.','
Перевод: Не используйте тестовые учётные данные в продакшене.'),
('it','staging','Развёртывание и эксплуатация
Пример: Test the migration in staging.','
Перевод: Протестируйте миграцию в стейджинге.'),
('it','configuration','Развёртывание и эксплуатация
Пример: Check the service configuration.','
Перевод: Проверьте конфигурацию сервиса.'),
('it','environment variable','Развёртывание и эксплуатация
Пример: Store the setting in an environment variable.','
Перевод: Сохраните настройку в переменной окружения.'),
('it','secret','Развёртывание и эксплуатация
Пример: Do not put secrets in the repository.','
Перевод: Не помещайте секреты в репозиторий.'),
('it','credential','Развёртывание и эксплуатация
Пример: The credentials are stored securely.','
Перевод: Учётные данные хранятся безопасно.'),
('it','container','Развёртывание и эксплуатация
Пример: Restart the container after updating the image.','
Перевод: Перезапустите контейнер после обновления образа.'),
('it','image','Развёртывание и эксплуатация
Пример: Build the image from the new source.','
Перевод: Соберите образ из нового исходного кода.'),
('it','volume','Развёртывание и эксплуатация
Пример: The database files are stored on a volume.','
Перевод: Файлы базы данных хранятся на томе.'),
('it','log','Развёртывание и эксплуатация
Пример: Check the logs for errors.','
Перевод: Проверьте логи на наличие ошибок.'),
('it','metric','Развёртывание и эксплуатация
Пример: This metric measures request latency.','
Перевод: Эта метрика измеряет задержку запросов.'),
('it','monitoring','Развёртывание и эксплуатация
Пример: Monitoring detected the outage.','
Перевод: Мониторинг обнаружил сбой.'),
('it','alert','Развёртывание и эксплуатация
Пример: The alert fired after repeated failures.','
Перевод: Тревога сработала после повторных неудач.'),
('it','outage','Развёртывание и эксплуатация
Пример: The outage lasted ten minutes.','
Перевод: Сбой длился десять минут.'),
('it','incident','Развёртывание и эксплуатация
Пример: We are investigating the incident.','
Перевод: Мы расследуем инцидент.'),
('it','uptime','Развёртывание и эксплуатация
Пример: We track service uptime.','
Перевод: Мы отслеживаем время безотказной работы сервиса.'),
('it','health check','Развёртывание и эксплуатация
Пример: The health check is failing.','
Перевод: Проверка состояния не проходит.'),
('it','load','Развёртывание и эксплуатация
Пример: Test the service under load.','
Перевод: Протестируйте сервис под нагрузкой.'),
('it','scaling','Развёртывание и эксплуатация
Пример: We need to plan for scaling.','
Перевод: Нужно планировать масштабирование.'),
('it','replica','Развёртывание и эксплуатация
Пример: The read replica is behind the primary.','
Перевод: Реплика чтения отстаёт от основной.'),
('it','queue','Развёртывание и эксплуатация
Пример: The worker reads from the queue.','
Перевод: Рабочий процесс читает из очереди.'),
('it','worker','Развёртывание и эксплуатация
Пример: The worker processes incoming jobs.','
Перевод: Рабочий процесс обрабатывает входящие задачи.'),
('it','scheduler','Развёртывание и эксплуатация
Пример: The scheduler runs the task daily.','
Перевод: Планировщик запускает задачу ежедневно.'),
('it','job','Развёртывание и эксплуатация
Пример: The job failed and will be retried.','
Перевод: Задача завершилась с ошибкой и будет повторена.'),
('it','pipeline','Развёртывание и эксплуатация
Пример: The pipeline runs the tests automatically.','
Перевод: Конвейер автоматически запускает тесты.'),
('it','continuous integration','Развёртывание и эксплуатация
Пример: Continuous integration checks each change.','
Перевод: Непрерывная интеграция проверяет каждое изменение.'),
('it','release notes','Развёртывание и эксплуатация
Пример: Read the release notes before upgrading.','
Перевод: Прочитайте примечания к выпуску перед обновлением.'),
('it','authentication','Безопасность и качество решений
Пример: Authentication failed.','
Перевод: Ошибка аутентификации.'),
('it','authorization','Безопасность и качество решений
Пример: Authorization is checked on the server.','
Перевод: Авторизация проверяется на сервере.'),
('it','permission','Безопасность и качество решений
Пример: The user lacks permission to edit this.','
Перевод: У пользователя нет прав на редактирование.'),
('it','role','Безопасность и качество решений
Пример: Assign the correct role to the account.','
Перевод: Назначьте правильную роль учетной записи.'),
('it','token','Безопасность и качество решений
Пример: The access token has expired.','
Перевод: Срок действия токена доступа истёк.'),
('it','encryption','Безопасность и качество решений
Пример: The connection uses encryption.','
Перевод: Соединение использует шифрование.'),
('it','hash','Безопасность и качество решений
Пример: Compare the file hash.','
Перевод: Сравните хеш файла.'),
('it','salt','Безопасность и качество решений
Пример: Use a unique salt for each password.','
Перевод: Используйте уникальную соль для каждого пароля.'),
('it','vulnerability','Безопасность и качество решений
Пример: The update fixes a vulnerability.','
Перевод: Обновление исправляет уязвимость.'),
('it','exploit','Безопасность и качество решений
Пример: The report describes a possible exploit.','
Перевод: Отчёт описывает возможный эксплойт.'),
('it','sanitize','Безопасность и качество решений
Пример: Sanitize untrusted HTML before rendering.','
Перевод: Очистите недоверенный HTML перед отображением.'),
('it','escape','Безопасность и качество решений
Пример: Escape text before inserting it into HTML.','
Перевод: Экранируйте текст перед вставкой в HTML.'),
('it','access control','Безопасность и качество решений
Пример: Review the access control rules.','
Перевод: Проверьте правила контроля доступа.'),
('it','least privilege','Безопасность и качество решений
Пример: Follow the principle of least privilege.','
Перевод: Следуйте принципу наименьших привилегий.'),
('it','audit','Безопасность и качество решений
Пример: Keep an audit log of sensitive changes.','
Перевод: Ведите журнал аудита чувствительных изменений.'),
('it','privacy','Безопасность и качество решений
Пример: Consider user privacy when logging data.','
Перевод: Учитывайте конфиденциальность пользователей при логировании данных.'),
('it','performance','Безопасность и качество решений
Пример: Measure performance before optimizing.','
Перевод: Измерьте производительность перед оптимизацией.'),
('it','benchmark','Безопасность и качество решений
Пример: Run the benchmark on the same machine.','
Перевод: Запускайте бенчмарк на той же машине.'),
('it','bottleneck','Безопасность и качество решений
Пример: The database is the bottleneck.','
Перевод: База данных является узким местом.'),
('it','optimization','Безопасность и качество решений
Пример: This optimization reduces allocations.','
Перевод: Эта оптимизация уменьшает количество выделений памяти.'),
('it','trade-off','Безопасность и качество решений
Пример: There is a trade-off between speed and memory use.','
Перевод: Существует компромисс между скоростью и использованием памяти.'),
('it','maintainability','Безопасность и качество решений
Пример: Clear names improve maintainability.','
Перевод: Понятные имена повышают поддерживаемость.'),
('it','readability','Безопасность и качество решений
Пример: This change improves readability.','
Перевод: Это изменение улучшает читаемость кода.'),
('it','reliability','Безопасность и качество решений
Пример: Retries can improve reliability.','
Перевод: Повторы могут повысить надёжность.'),
('it','scalability','Безопасность и качество решений
Пример: We need to test scalability.','
Перевод: Нужно протестировать масштабируемость.'),
('it','robustness','Безопасность и качество решений
Пример: Input checks improve robustness.','
Перевод: Проверка входных данных повышает надёжность.'),
('it','idempotent','Безопасность и качество решений
Пример: The operation should be idempotent.','
Перевод: Операция должна быть идемпотентной.'),
('it','atomic','Безопасность и качество решений
Пример: The update must be atomic.','
Перевод: Обновление должно быть атомарным.'),
('it','deterministic','Безопасность и качество решений
Пример: The test should be deterministic.','
Перевод: Тест должен быть детерминированным.'),
('it','could you clarify the requirement?','Фразы для рабочего общения
Пример: Could you clarify the requirement?','
Перевод: Не могли бы вы уточнить требование?'),
('it','i can reproduce the issue.','Фразы для рабочего общения
Пример: I can reproduce the issue.','
Перевод: Я могу воспроизвести проблему.'),
('it','i cannot reproduce it locally.','Фразы для рабочего общения
Пример: I cannot reproduce it locally.','
Перевод: Мне не удаётся воспроизвести это локально.'),
('it','what is the expected behavior?','Фразы для рабочего общения
Пример: What is the expected behavior?','
Перевод: Каково ожидаемое поведение?'),
('it','this change is ready for review.','Фразы для рабочего общения
Пример: This change is ready for review.','
Перевод: Это изменение готово к проверке.'),
('it','i need more time to investigate.','Фразы для рабочего общения
Пример: I need more time to investigate.','
Перевод: Мне нужно больше времени для расследования.'),
('it','i am blocked by this issue.','Фразы для рабочего общения
Пример: I am blocked by this issue.','
Перевод: Я заблокирован этой проблемой.'),
('it','could you share the logs?','Фразы для рабочего общения
Пример: Could you share the logs?','
Перевод: Не могли бы вы предоставить логи?'),
('it','the tests pass locally.','Фразы для рабочего общения
Пример: The tests pass locally.','
Перевод: Тесты проходят локально.'),
('it','this is outside the current scope.','Фразы для рабочего общения
Пример: This is outside the current scope.','
Перевод: Это выходит за рамки текущего объёма работ.'),
('it','what are the trade-offs?','Фразы для рабочего общения
Пример: What are the trade-offs?','
Перевод: Каковы компромиссы?'),
('it','let us verify the assumption.','Фразы для рабочего общения
Пример: Let us verify the assumption.','
Перевод: Давайте проверим предположение.'),
('it','we need a rollback plan.','Фразы для рабочего общения
Пример: We need a rollback plan.','
Перевод: Нужен план отката.'),
('it','i will update the documentation.','Фразы для рабочего общения
Пример: I will update the documentation.','
Перевод: Я обновлю документацию.'),
('it','thanks for catching that.','Фразы для рабочего общения
Пример: Thanks for catching that.','
Перевод: Спасибо, что заметили это.')
) AS v(course, english, original, suffix)
WHERE w.topic = v.course AND lower(trim(w.english)) = v.english
  AND w.definition_ru = v.original;
