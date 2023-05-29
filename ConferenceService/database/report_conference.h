#ifndef REPORTCONFERENCE_H
#define REPORTCONFERENCE_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class ReportConference{
        private:
            long _conf_id;
            long _report_id;
        public:
	    static void init();
            static ReportConference fromJSON(const std::string & str);

            long &conf_id();
            long &report_id();
            
            void add_report_to_conf(long report_id, long conf_id);
            std::vector<ReportConference> read_all_reports_from_conf(long conf_id);

            Poco::JSON::Object::Ptr toJSON() const;

    };
}

#endif