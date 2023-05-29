#include "conference.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Conference::init()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Conference`(`id` int NOT NULL AUTO_INCREMENT,"
                        << "`name_conf`    varchar(1024) NOT NULL,"
                        << "`organizer_id` int NOT NULL,"
                        << "`category_id`  int NOT NULL,"
                        << "`description`  text NOT NULL,"
                        << "`date_of_conf` text NOT NULL,"

                        << "PRIMARY KEY (`id`),"
                        << "KEY `FK_2` (`category_id`),"
                        << "CONSTRAINT `FK_1` FOREIGN KEY `FK_2` (`category_id`) REFERENCES `Category` (`id`));",
                now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Conference::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("name_conf", _name_conf);
        root->set("organizer_id", _organizer_id);
        root->set("category_id", _category_id);
        root->set("description", _description);
        root->set("date_of_conf", _date_of_conf);

        return root;
    }

    Conference Conference::fromJSON(const std::string &str)
    {
        Conference conf;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        conf.id() = object->getValue<long>("id");
        conf.name_conf() = object->getValue<std::string>("name_conf");
        conf.organizer_id() = object->getValue<long>("organizer_id");
        conf.category_id() = object->getValue<long>("category_id");
        conf.description() = object->getValue<std::string>("description");
        conf.date_of_conf() = object->getValue<std::string>("date_of_conf");

        return conf;
    }


    void Conference::add_conf(std::string name_conf, long organizer_id, long category_id, std::string description, std::string date_of_conf)
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Conference (name_conf, organizer_id, category_id, description, date_of_conf) VALUES(?, ?, ?, ?, ?)",
                use(name_conf),
                use(organizer_id),
                use(category_id),
                use(description),
                use(date_of_conf);

            insert.execute();

            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {
            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    long &Conference::id()
    {
        return _id;
    }

    std::string &Conference::name_conf()
    {
        return _name_conf;
    }

    long &Conference::organizer_id()
    {
        return _organizer_id;
    }

    long &Conference::category_id()
    {
        return _category_id;
    }
    
    std::string &Conference::description()
    {
        return _description;
    }

    std::string &Conference::date_of_conf()
    {
        return _date_of_conf;
    }
}