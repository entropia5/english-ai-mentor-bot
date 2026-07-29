#include "core/text.h"
#include "file_utils.h"
#include "logger.h"
#include "services/vocabulary_service.h"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <set>

using json = nlohmann::json;
namespace fs = std::filesystem;

std::vector<GeneratedWord> built_in_fallback_practical_words() {
    return {{"receipt", "/rɪˈsiːt/", "рисит", "чек"},
            {"refund", "/ˈriːfʌnd/", "рифанд", "возврат денег"},
            {"warranty", "/ˈwɒrənti/", "уорэнти", "гарантия"},
            {"invoice", "/ˈɪnvɔɪs/", "инвойс", "счет"},
            {"estimate", "/ˈestɪmət/", "эстимэт", "оценка стоимости"},
            {"deadline", "/ˈdedlaɪn/", "дедлайн", "крайний срок"},
            {"appointment", "/əˈpɔɪntmənt/", "эпойнтмэнт", "встреча, запись"},
            {"schedule", "/ˈʃedjuːl/", "шеджул", "расписание"},
            {"reminder", "/rɪˈmaɪndər/", "римайндер", "напоминание"},
            {"checklist", "/ˈtʃeklɪst/", "чеклист", "контрольный список"},
            {"paperwork", "/ˈpeɪpərwɜːrk/", "пэйпэруорк", "документы"},
            {"timesheet", "/ˈtaɪmʃiːt/", "таймшит", "табель учета времени"},
            {"courier", "/ˈkʊriər/", "куриэр", "курьер"},
            {"parcel", "/ˈpɑːrsl/", "парсл", "посылка"},
            {"delivery", "/dɪˈlɪvəri/", "диливэри", "доставка"},
            {"package", "/ˈpækɪdʒ/", "пэкидж", "упаковка, посылка"},
            {"storage", "/ˈstɔːrɪdʒ/", "сторидж", "хранение"},
            {"shelf", "/ʃelf/", "шелф", "полка"},
            {"drawer", "/drɔːr/", "дроэр", "ящик"},
            {"outlet", "/ˈaʊtlet/", "аутлет", "розетка"},
            {"adapter", "/əˈdæptər/", "эдэптэр", "адаптер"},
            {"blanket", "/ˈblæŋkɪt/", "блэнкит", "одеяло"},
            {"kettle", "/ˈketl/", "кетл", "чайник"},
            {"faucet", "/ˈfɔːsɪt/", "фосит", "кран"},
            {"laundry", "/ˈlɔːndri/", "лондри", "стирка"},
            {"detergent", "/dɪˈtɜːrdʒənt/", "дитёрджэнт", "моющее средство"},
            {"commute", "/kəˈmjuːt/", "комьют", "поездка на работу"},
            {"shortcut", "/ˈʃɔːrtkʌt/", "шорткат", "короткий путь"},
            {"suburb", "/ˈsʌbɜːrb/", "сабёрб", "пригород"},
            {"traffic", "/ˈtræfɪk/", "трэфик", "дорожное движение"},
            {"luggage", "/ˈlʌɡɪdʒ/", "лагидж", "багаж"},
            {"boarding", "/ˈbɔːrdɪŋ/", "бординг", "посадка"},
            {"arrival", "/əˈraɪvəl/", "эрайвэл", "прибытие"},
            {"departure", "/dɪˈpɑːrtʃər/", "дипарчер", "отправление"},
            {"platform", "/ˈplætfɔːrm/", "плэтформ", "платформа"},
            {"aisle", "/aɪl/", "айл", "проход"},
            {"receipt", "/rɪˈsiːt/", "рисит", "чек"},
            {"ingredient", "/ɪnˈɡriːdiənt/", "ингридиэнт", "ингредиент"},
            {"leftover", "/ˈleftoʊvər/", "лэфтоувэр", "остаток еды"},
            {"portion", "/ˈpɔːrʃn/", "поршн", "порция"},
            {"utensil", "/juːˈtensl/", "ютенсл", "столовый прибор"},
            {"grocery", "/ˈɡroʊsəri/", "гроусэри", "продукты"},
            {"pantry", "/ˈpæntri/", "пэнтри", "кладовая"},
            {"receipt", "/rɪˈsiːt/", "рисит", "чек"},
            {"summary", "/ˈsʌməri/", "самэри", "краткое описание"},
            {"request", "/rɪˈkwest/", "риквест", "запрос"},
            {"approval", "/əˈpruːvəl/", "эпрувэл", "одобрение"},
            {"feedback", "/ˈfiːdbæk/", "фидбэк", "обратная связь"},
            {"meeting", "/ˈmiːtɪŋ/", "митинг", "встреча"},
            {"agenda", "/əˈdʒendə/", "эдженда", "повестка"},
            {"priority", "/praɪˈɔːrəti/", "прайорити", "приоритет"},
            {"progress", "/ˈprɑːɡres/", "прогрэс", "прогресс"},
            {"issue", "/ˈɪʃuː/", "ишью", "проблема"},
            {"solution", "/səˈluːʃn/", "солюшн", "решение"},
            {"backup", "/ˈbækʌp/", "бэкап", "резервная копия"},
            {"folder", "/ˈfoʊldər/", "фоулдэр", "папка"},
            {"attachment", "/əˈtætʃmənt/", "этэчмэнт", "вложение"},
            {"browser", "/ˈbraʊzər/", "браузэр", "браузер"},
            {"password", "/ˈpæswɜːrd/", "пэсуорд", "пароль"},
            {"privacy", "/ˈpraɪvəsi/", "прайвэси", "конфиденциальность"}};
}

std::string json_string_value(const json& item, const std::string& primary_key,
                              const std::string& fallback_key = "") {
    if (item.contains(primary_key) && item[primary_key].is_string()) {
        return trim(item[primary_key].get<std::string>());
    }
    if (!fallback_key.empty() && item.contains(fallback_key) && item[fallback_key].is_string()) {
        return trim(item[fallback_key].get<std::string>());
    }
    return "";
}

std::vector<GeneratedWord> load_fallback_words_from_file(const fs::path& path) {
    std::string text = read_text_file(path.string());
    if (text.empty()) {
        return {};
    }

    try {
        json document = json::parse(text);
        const json* words_json = &document;
        if (document.is_object() && document.contains("words")) {
            words_json = &document["words"];
        }
        if (!words_json->is_array()) {
            LOG_WARNING("Fallback dictionary is not a JSON array: " + path.string());
            return {};
        }

        std::vector<GeneratedWord> words;
        std::set<std::string> seen;
        for (size_t i = 0; i < words_json->size(); i++) {
            const json& item = (*words_json)[i];
            if (!item.is_object()) {
                LOG_WARNING("Skipping fallback dictionary item " + std::to_string(i) +
                            ": expected object");
                continue;
            }

            GeneratedWord word;
            word.english = to_lower_ascii(json_string_value(item, "english", "word"));
            word.transcription = json_string_value(item, "transcription", "trans");
            word.pronunciation = json_string_value(item, "pronunciation_ru", "pronunciation");
            word.translation = json_string_value(item, "translation_ru", "translation");
            word.definition = json_string_value(item, "definition_ru", "definition");

            if (word.english.empty() || word.transcription.empty() || word.pronunciation.empty() ||
                word.translation.empty() || word.definition.empty() ||
                is_suspicious_generated_word(word.english)) {
                LOG_WARNING("Skipping invalid fallback dictionary word at index " +
                            std::to_string(i) + " in " + path.string());
                continue;
            }
            if (seen.count(word.english) > 0) {
                LOG_WARNING("Skipping duplicate fallback dictionary word: " + word.english);
                continue;
            }

            seen.insert(word.english);
            words.push_back(word);
        }

        LOG("Loaded fallback dictionary from " + path.string() + ": " +
            std::to_string(words.size()) + " words");
        return words;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse fallback dictionary " + path.string() + ": " + e.what());
        return {};
    }
}

std::vector<GeneratedWord> fallback_practical_words() {
    std::vector<fs::path> paths = {fs::path(project_data_dir()) / "fallback_words.json",
                                   fs::path("resources") / "fallback_words.json",
                                   fs::path("..") / "resources" / "fallback_words.json"};

    for (const fs::path& path : paths) {
        if (!fs::exists(path)) {
            continue;
        }
        auto words = load_fallback_words_from_file(path);
        if (!words.empty()) {
            return words;
        }
    }

    LOG_WARNING("Fallback dictionary file is unavailable, using built-in fallback words");
    return built_in_fallback_practical_words();
}
