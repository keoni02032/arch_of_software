#ifndef REPORT_H
#define REPORT_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Report{
        private:
            long _id;
            std::string _name_report;
            long _author_id;
            std::string _annotation;
            std::string _text_report;
            std::string _date_creation;

        public:
            static Report fromJSON(const std::string & str);

            long& id();
            std::string &name_report();
            long& author_id();
            std::string &annotation();
            std::string &text_report();
            std::string &date_creation();

            static void init();
            std::vector<Report> read_all_reports();
            std::optional<Report> search_report(long id);
            void add_report(std::string name_report, long author_id, std::string annotation, std::string text_report, std::string date_creation);

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif