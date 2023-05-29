#include "user.h"
#include "database.h"
#include "../config/config.h"
#include "cache.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>
#include <algorithm>
#include <future>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void User::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            for (auto &hint : database::Database::get_all_hints()) {

                Statement create_stmt(session);
                
                create_stmt << "CREATE TABLE IF NOT EXISTS `User` (`id` INT NOT NULL AUTO_INCREMENT,"
                            << "`main_id` INT NOT NULL,"
                            << "`first_name` VARCHAR(256) NOT NULL,"
                            << "`last_name` VARCHAR(256) NOT NULL,"
                            << "`login` VARCHAR(256) NOT NULL,"
                            << "`password` VARCHAR(256) NOT NULL,"
                            << "`email` VARCHAR(256) NULL,"
                            << "`title` VARCHAR(1024) NULL,"
                            << "PRIMARY KEY (`id`), KEY `mid` (`main_id`), KEY `fn` (`first_name`),KEY `ln` (`last_name`));"
                            << hint,
                    now;

                std::cout << create_stmt.toString() << std::endl;
            }
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

    std::optional<User> User::read_from_cache_by_id(long main_id)
    {

        try
        {
            std::string result;
            if (database::Cache::get().get(main_id, result))
                return fromJSON(result);
            else
                return std::optional<User>();
        }
        catch (std::exception& err)
        {
           // std::cerr << "error:" << err.what() << std::endl;
            return std::optional<User>();
        }
    }

    void User::save_to_cache()
    {
        std::stringstream ss;
        Poco::JSON::Stringifier::stringify(toJSON(), ss);
        std::string message = ss.str();
        database::Cache::get().put(_main_id, message);
    }

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("main_id", _main_id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("title", _title);
        root->set("login", _login);
        root->set("password", _password);

        return root;
    }

    User User::fromJSON(const std::string &str)
    {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        user.id() = object->getValue<long>("id");
        user.main_id() = object->getValue<long>("main_id");
        user.first_name() = object->getValue<std::string>("first_name");
        user.last_name() = object->getValue<std::string>("last_name");
        user.email() = object->getValue<std::string>("email");
        user.title() = object->getValue<std::string>("title");
        user.login() = object->getValue<std::string>("login");
        user.password() = object->getValue<std::string>("password");

        return user;
    }

    std::optional<long> User::auth(std::string &login, std::string &password)
    {
        try
        {
            std::vector<long> result;
            std::vector<std::string> hints = database::Database::get_all_hints();

            std::vector<std::future<std::vector<long>>> futures;
            for (const std::string &hint : hints)
            {
                auto handle = std::async(std::launch::async, [login, password, hint]() mutable -> std::vector<long>
                                        {
                                            std::vector<long> result;

                                            Poco::Data::Session session = database::Database::get().create_session();
                                            Statement select(session);
                                            std::string select_str = "SELECT main_id FROM User where login='";
                                            select_str += login;
                                            select_str += "' and password='";
                                            select_str += password;
                                            select_str += "'";
                                            select_str += hint;
                                            select << select_str;
                                            std::cout << select_str << std::endl;
                                            
                                            select.execute();
                                            Poco::Data::RecordSet rs(select);

                                            bool more = rs.moveFirst();
                                            while (more)
                                            {
                                                long a;
                                                a = rs[0].convert<long>();
                                                result.push_back(a);
                                                more = rs.moveNext();
                                            }
                                            return result; });

                futures.emplace_back(std::move(handle));
            }


            for (std::future<std::vector<long>> &res : futures)
            {
                std::vector<long> v = res.get();
                std::copy(std::begin(v),
                        std::end(v),
                        std::back_inserter(result));
            }
            if (result.size()) return result[0];
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
        }
        return {};
    }
    
    std::optional<User> User::read_by_id(long id)
    {
        try
        {
            std::string hint = database::Database::sharding_hint(id);
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            User a;
            select << "SELECT id, main_id, first_name, last_name, email, title,login,password FROM User where main_id=?" << hint,
                into(a._id),
                into(a._main_id),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._title),
                into(a._login),
                into(a._password),
                use(id),
                range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }

    std::vector<User> User::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            std::vector<User> result;

            for (auto &hint : database::Database::get_all_hints()) {
                Statement select(session);
                User a;
                select << "SELECT id, main_id, first_name, last_name, email, title, login, password FROM User" << hint,
                    into(a._id),
                    into(a._main_id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._title),
                    into(a._login),
                    into(a._password),
                    range(0, 1); //  iterate over result set one row at a time

                while (!select.done())
                {
                    if (select.execute())
                        result.push_back(a);
                }
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

    std::vector<User> User::search(std::string first_name, std::string last_name)
    {
        try
        {
            std::vector<User> result;

            std::vector<std::future<std::vector<User>>> futures;
            first_name += "%";
            last_name += "%";
            
            for (auto &hint : database::Database::get_all_hints()) {
                auto handle = std::async(std::launch::async, [first_name, last_name, hint]() mutable -> std::vector<User>
                {
                    std::vector<User> result;
                    
                    Poco::Data::Session session = database::Database::get().create_session();
                    Statement select(session);
                    
                    User a;
                    select << "SELECT id, main_id, first_name, last_name, email, title, login, password FROM User where first_name LIKE ? and last_name LIKE ?" << hint,
                        into(a._id),
                        into(a._main_id),
                        into(a._first_name),
                        into(a._last_name),
                        into(a._email),
                        into(a._title),
                        into(a._login),
                        into(a._password),
                        use(first_name),
                        use(last_name),
                        range(0, 1); //  iterate over result set one row at a time

                        Poco::Data::RecordSet rs(select);
                                        
                        bool more = rs.moveFirst();
                        while (more)
                        {
                            User a;
                            a._id = rs[0].convert<long>();
                            a._main_id = rs[1].convert<long>();
                            a._first_name = rs[2].convert<std::string>();
                            a._last_name = rs[3].convert<std::string>();
                            a._email = rs[4].convert<std::string>();
                            a._login = rs[5].convert<std::string>();
                            a._password = rs[6].convert<std::string>();
                            result.push_back(a);
                            more = rs.moveNext();
                        }
                        return result; });
                futures.emplace_back(std::move(handle));       
            }

            for (std::future<std::vector<User>> &res : futures)
            {
                std::vector<User> v = res.get();
                std::copy(std::begin(v),
                        std::end(v),
                        std::back_inserter(result));
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

    std::optional<User> User::read_by_login(std::string &login)
    {
        try
        {
            std::vector<User> result;
            std::vector<std::string> hints = database::Database::get_all_hints();

            std::vector<std::future<std::vector<User>>> futures;
            for (const std::string &hint : hints)
            {
                auto handle = std::async(std::launch::async, [login, hint]() mutable -> std::vector<User>
                                        {
                                            std::vector<User> result;

                                            Poco::Data::Session session = database::Database::get().create_session();
                                            Statement select(session);
                                            std::string select_str = "SELECT main_id, first_name, last_name, title FROM User where login='";
                                            select_str += login;
                                            select_str += "'";
                                            select_str += hint;
                                            select << select_str;
                                            std::cout << select_str << std::endl;
                                            
                                            select.execute();
                                            Poco::Data::RecordSet rs(select);

                                            bool more = rs.moveFirst();
                                            while (more)
                                            {
                                                User a;
                                                a._main_id = rs[0].convert<long>();
                                                a._first_name = rs[1].convert<std::string>();
                                                a._last_name = rs[2].convert<std::string>();
                                                a._title = rs[3].convert<std::string>();
                                                result.push_back(a);
                                                more = rs.moveNext();
                                            }
                                            return result; });

                futures.emplace_back(std::move(handle));
            }

            for (std::future<std::vector<User>> &res : futures)
            {
                std::vector<User> v = res.get();
                std::copy(std::begin(v),
                        std::end(v),
                        std::back_inserter(result));
            }
            if (result.size()) return result[0];
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            
        }
        return {};
    }

    long User::get_len_database()
    {
        long result = 0;
        std::vector<std::string> hints = database::Database::get_all_hints();
        for (const std::string &hint : hints)
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            long a;
            std::string select_str = "SELECT COUNT(*) FROM User";
            select_str += hint;
            select << select_str,
                into(a),
                range(0, 1);

            select.execute();
            Poco::Data::RecordSet rs(select);

            if (rs.moveFirst()) {
                result += a;
            }
        }
        return result;
    }

    void User::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();

            long len_database = User::get_len_database();
            len_database +=1;
            std::string hint = database::Database::sharding_hint(len_database);

            Poco::Data::Statement insert(session);

            insert << "INSERT INTO User (main_id, first_name,last_name,email,title,login,password) VALUES(?, ?, ?, ?, ?, ?, ?)" << hint,
                use(len_database),
                use(_first_name),
                use(_last_name),
                use(_email),
                use(_title),
                use(_login),
                use(_password);

            insert.execute();
            //std::cout << "inserted:" << _id << std::endl;
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

    const std::string &User::get_login() const
    {
        return _login;
    }

    const std::string &User::get_password() const
    {
        return _password;
    }

    std::string &User::login()
    {
        return _login;
    }

    std::string &User::password()
    {
        return _password;
    }

    long User::get_id() const
    {
        return _id;
    }

    long User::get_main_id() const
    {
        return _main_id;
    }

    const std::string &User::get_first_name() const
    {
        return _first_name;
    }

    const std::string &User::get_last_name() const
    {
        return _last_name;
    }

    const std::string &User::get_email() const
    {
        return _email;
    }

    const std::string &User::get_title() const
    {
        return _title;
    }

    long &User::id()
    {
        return _id;
    }

    long &User::main_id()
    {
        return _main_id;
    }

    std::string &User::first_name()
    {
        return _first_name;
    }

    std::string &User::last_name()
    {
        return _last_name;
    }

    std::string &User::email()
    {
        return _email;
    }

    std::string &User::title()
    {
        return _title;
    }
}