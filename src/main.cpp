#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::json;

// Структура для хранения данных о прогнозе погоды на один день
struct DailyWeatherForecast {
    std::string date;
    double minTemperature;
    double maxTemperature;
    std::string weatherDescription;
};

// функция для обработки HTTP-ответа от сервера
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Функция для получения прогноза погоды 
std::string getWeatherForecast(const std::string& city) {
    std::string apiKey = "5ae2ce314387f8118ec73263ed1ff985"; 
    std::string apiUrl = "https://api.openweathermap.org/data/2.5/forecast?q=" + city + "&appid=" + apiKey;

    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return response;
}

int main() {
    std::string city;
    std::cout << "Введите название города: ";
    std::cin >> city;

    std::string weatherData = getWeatherForecast(city);

    try {
        json jsonData = json::parse(weatherData);

        // Обработка данных и заполнение списка прогнозов
        std::vector<DailyWeatherForecast> forecasts;

        for (const auto& forecast : jsonData["list"]) {
            DailyWeatherForecast dailyForecast;
            dailyForecast.date = forecast["dt_txt"];
            dailyForecast.minTemperature = forecast["main"]["temp_min"];
            dailyForecast.maxTemperature = forecast["main"]["temp_max"];
            dailyForecast.weatherDescription = forecast["weather"][0]["description"];
            forecasts.push_back(dailyForecast);
        }

        // Пользовательское меню
        int choice;
        do {
            std::cout << "\nМеню:\n";
            std::cout << "1. Просмотр прогноза на все 5 дней с показом времени " << city << "\n";
            std::cout << "2. Просмотр прогноза на конкретный день\n";
            std::cout << "3. Отображение самой низкой и самой высокой температур\n";
            std::cout << "0. Выход\n";
            std::cout << "Выберите действие: ";
            std::cin >> choice;

            switch (choice) {
                case 1:
                    // Вывод прогноза на все 5 дней с показом времени
                    for (const auto& forecast : forecasts) {
                        std::cout << "Дата: " << forecast.date << std::endl;
                        std::cout << "Температура: " << std::endl;
                        std::cout << forecast.minTemperature << " K - " << forecast.maxTemperature << " K" << std::endl;
                        std::cout << "Описание: " << forecast.weatherDescription << "\n";
                    }
                    break;
                case 2: {
                    // Вывод прогноза на конкретный день
                    std::string selectedDate;
                    std::cout << "Введите дату в формате ГГГГ-ММ-ДД: ";
                    std::cin >> selectedDate;
                    for (const auto& forecast : forecasts) {
                        if (forecast.date.find(selectedDate) != std::string::npos) {
                           std::cout << "Дата: " << forecast.date << std::endl;
                           std::cout << "Температура: " << std::endl;
                           std::cout << forecast.minTemperature << " K - " << forecast.maxTemperature << " K" << std::endl;
                           std::cout << "Описание: " << forecast.weatherDescription << "\n";
                        }
                    }
                    break;
                }
                case 3: {
                    // Отображение самой низкой и самой высокой температур
                    double minTemp = std::numeric_limits<double>::max();
                    double maxTemp = std::numeric_limits<double>::min();

                    for (const auto& forecast : forecasts) {
                        if (forecast.minTemperature < minTemp)
                            minTemp = forecast.minTemperature;
                        if (forecast.maxTemperature > maxTemp)
                            maxTemp = forecast.maxTemperature;
                    }

                    std::cout << "Самая низкая температура: " << minTemp << " K\n";
                    std::cout << "Самая высокая температура: " << maxTemp << " K\n";
                    break;
                }
                case 0:
                    std::cout << "Выход из программы.\n";
                    break;
                default:
                    std::cout << "Неверный ввод. Попробуйте снова.\n";
            }
        } while (choice != 0);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при обработке данных: " << e.what() << "\n";
    }

    return 0;
}

