# English Mentor Bot 🤖

Telegram bot для изучения английского языка с AI, PostgreSQL и системой повторения слов.

## Возможности

- 📚 **Словарь**
- ✅ **Выученные слова**
- 🤖 **AI помощник** - ответы на вопросы через Groq AI
- 📊 **Статистика**
- 🌅 **Утренняя рассылка**
- 🏳️ **Транскрипции**

## Технологии

- C++17
- PostgreSQL
- Groq AI API
- Telegram Bot API
- libcurl, nlohmann/json

## Установка

1. Клонируйте репозиторий
2. Скопируйте `.env.example` в `.env` и заполните токены
3. Установите зависимости: `sudo apt-get install libpqxx-dev libcurl4-openssl-dev nlohmann-json3-dev`
4. Соберите проект:

```bash
mkdir build && cd build
cmake ..
make
./english_mentor
