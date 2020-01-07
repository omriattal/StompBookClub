#ifndef BOOST_ECHO_CLIENT_STOMPFRAME_H
#define BOOST_ECHO_CLIENT_STOMPFRAME_H


#include <string>
#include <map>
#include <vector>

class StompFrame {
private:
    std::string command;
    std::map<std::string,std::string> headers;
    std::string body;
    static std::vector<std::string> split(const std::string &s, char delimiter);

public:
    StompFrame();

    void addHeader(std::string key, std::string value);

    std::string getHeader(std::string key);


    static StompFrame & parseString(std::string message);

    const std::string &getCommand() const;

    void setCommand(const std::string &command);

    const std::map<std::string, std::string> &getHeaders() const;

    void setHeaders(const std::map<std::string, std::string> &headers);

    const std::string &getBody() const;

    void setBody(const std::string &body);

    std::string toString();
};



#endif //BOOST_ECHO_CLIENT_STOMPFRAME_H