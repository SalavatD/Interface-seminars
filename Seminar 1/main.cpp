#include <fstream>
#include <bitset>
#include <vector>
#include <iostream>
#include <sstream>

/*
 *
 * Вариант 6
 * m = 9
 * n = 3
 *
 */

// Проверка целостности сообщения
template<typename T>
void check_message(std::vector<T>);

// Вывод в stdout и файл
template<typename T>
void print_message(std::vector<T>, const std::string &);

int main() {
    const int packet_size = 8;
    const int count_of_packets = 3; // !!! count_of_packets <= 4 !!!

    const std::string input_file_name = "input_data.txt";
    const std::string output_file_name = "output_message.txt";

    std::ifstream ifs(input_file_name);
    uint32_t buffer;
    ifs >> std::hex >> buffer;

    std::bitset<packet_size * count_of_packets> bytes(buffer);
    std::vector<std::bitset<packet_size>> packets(count_of_packets);

    for (size_t i = 0; i < count_of_packets; ++i) {
        auto mask = std::bitset<packet_size * count_of_packets>(0xFF * (1u << 8 * i));
        auto packet = (bytes & mask) >> (packet_size * i);
        packets[count_of_packets - i - 1] = std::bitset<packet_size>(packet.to_ulong());
    }

    try {
        check_message(packets);
        print_message(packets, output_file_name);
    } catch (std::invalid_argument &invalid_argument) {
        std::cout << invalid_argument.what() << std::endl;
    }

    return 0;
}

/*
 * Реализуемый алгоритм:
 *
 * ****************************
 * | 7  6  5 | 4  3  2 | 1  0 |
 * ----------------------------
 * | a  b  c | d  e  f | n  n |
 * | ^  ^  ^ | ^  ^  ^ | ^  ^ |
 * |полезные |контроль-|номер |
 * |данные   |ные биты |пакета|
 * ****************************
 *
 * d = a xor b
 * e = b xor c
 * f = a xor c
 *
 *
 * Для входных данных:
 * 2C95E2
 * Правильный вывод:
 * BIN: 001100111
 * HEX: 67
 *
 */

template<typename T>
void check_message(std::vector<T> packets) {
    uint32_t counter = 0;
    for (const auto &packet : packets) {
        uint32_t number_of_packet = (packet & std::bitset<8>(0x3)).to_ulong();
        if (counter != number_of_packet) {
            std::stringstream exception_message;
            exception_message << "Invalid number of packet! Received: ";
            exception_message << number_of_packet;
            exception_message << " Expected: " << counter;
            throw std::invalid_argument(exception_message.str());
        }
        if (packet[4] != (packet[7] ^ packet[6])) {
            std::stringstream exception_message;
            exception_message << "Invalid first or second byte of message or first control byte! ";
            exception_message << "Packet: " << number_of_packet;
            throw std::invalid_argument(exception_message.str());
        }
        if (packet[3] != (packet[6] ^ packet[5])) {
            std::stringstream exception_message;
            exception_message << "Invalid second or third byte of message or second control byte!";
            exception_message << "Packet: " << number_of_packet;
            throw std::invalid_argument(exception_message.str());
        }
        if (packet[2] != (packet[7] ^ packet[5])) {
            std::stringstream exception_message;
            exception_message << "Invalid first or third byte of message or third control byte!";
            exception_message << "Packet: " << number_of_packet;
            throw std::invalid_argument(exception_message.str());
        }
        counter++;
    }
}

template<typename T>
void print_message(std::vector<T> packets, const std::string &file_name) {
    std::ofstream ofs(file_name);
    std::stringstream output_message_stream;
    for (const auto &packet : packets) {
        output_message_stream << std::bitset<3>(((packet & std::bitset<8>(0xE0)) >> 5).to_ulong());
    }
    std::bitset<12> output_message(output_message_stream.str());
    std::cout << output_message << std::endl << std::hex << output_message.to_ulong() << std::endl;
    ofs << output_message << std::endl << std::hex << output_message.to_ulong() << std::endl;
}
