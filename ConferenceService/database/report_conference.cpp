#include "report_conference.h"
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

    void ReportConference::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt <<"CREATE TABLE IF NOT EXISTS `ReportConference`(`report_id` int NOT NULL ,"
                        <<"`conf_id`   int NOT NULL ,"

                        <<"PRIMARY KEY (`report_id`, `conf_id`),"
                        <<"KEY `FK_1` (`report_id`),"
                        <<"CONSTRAINT `FK_4` FOREIGN KEY `FK_1` (`report_id`) REFERENCES `Report` (`id`),"
                        <<"KEY `FK_3` (`conf_id`),"
                        <<"CONSTRAINT `FK_5` FOREIGN KEY `FK_3` (`conf_id`) REFERENCES `Conference` (`id`));",
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

    Poco::JSON::Object::Ptr ReportConference::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("report_id", _report_id);
        root->set("conf_id", _conf_id);

        return root;
    }

    ReportConference ReportConference::fromJSON(const std::string &str)
    {
        ReportConference reportconf;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        reportconf.report_id() = object->getValue<long>("report_id");
        reportconf.conf_id() = object->getValue<long>("conf_id");

        return reportconf;
    }

    void ReportConference::add_report_to_conf(long report_id, long conf_id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO ReportConference (report_id, conf_id) VALUES(?, ?)",
                use(report_id),
                use(conf_id);

            insert.execute();

            std::cout << "inserted: " << _report_id << " & " << _conf_id << std::endl;
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

    std::vector<ReportConference> ReportConference::read_all_reports_from_conf(long conf_id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<ReportConference> result;
            ReportConference a;
            select << "SELECT report_id, conf_id FROM ReportConference where conf_id=?",
                into(a._report_id),
                use(conf_id),
                range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
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

    long &ReportConference::report_id()
    {
        return _report_id;
    }

    long &ReportConference::conf_id()
    {
        return _conf_id;
    }

}