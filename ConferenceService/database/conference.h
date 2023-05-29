#ifndef CONFERENCE_H
#define CONFERENCE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Conference{
        private:
            long _id;
            std::string _name_conf;
            long _organizer_id;
            long _category_id;
            std::string _description;
            std::string _date_of_conf;

        public:
            static void init();
            static Conference fromJSON(const std::string & str);

            long &id();
            std::string &name_conf();
            long &organizer_id();
            long &category_id();
            std::string &description();
            std::string &date_of_conf();

            void add_conf(std::string name_conf, long organizer_id, long category_id, std::string description, std::string date_of_conf);

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif