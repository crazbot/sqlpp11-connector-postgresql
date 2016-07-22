#include <sqlpp11/postgresql/postgresql.h>

#include "assertThrow.hpp"

#include "TabFoo.h"
#include "TabBar.h"

namespace sql = sqlpp::postgresql;
int Exceptions(int , char **) {
    {
        //broken_connection exception on bad config
        auto config = std::make_shared<sql::connection_config>();
        assert_throw(sql::connection db(config), sql::broken_connection );
    }

    model::TabFoo foo;
    model::TabBar bar;
    auto config = std::make_shared<sql::connection_config>();
    config->user = "postgres";
    config->password="postgres";
    config->dbname = "test";
    config->host = "localhost";
    config->port = 5432;
    config->debug = true;

    try
    {
        sql::connection db(config);
    }
    catch (const sql::broken_connection &){
        std::cerr << "For testing, you'll need to create a database sqlpp_postgresql" << std::endl;
        throw;
    }

    sql::connection db(config);
    try{
        db.execute(R"(DROP TABLE IF EXISTS tabfoo;)");
        db.execute(R"(CREATE TABLE tabfoo
                   (
                   alpha bigserial NOT NULL,
                   beta smallint UNIQUE,
                   gamma text CHECK( length(gamma) < 5 ),
                   c_bool boolean,
                   c_timepoint timestamp with time zone DEFAULT now(),
                   c_day date
                   ))");

        assert_throw( db(insert_into(foo).set(foo.beta = std::numeric_limits<int16_t>::max() + 1 ) ), sql::data_exception);
        assert_throw( db(insert_into(foo).set(foo.gamma = "123456")), sql::check_violation );
        db(insert_into(foo).set(foo.beta = 5 ) );
        assert_throw( db(insert_into(foo).set(foo.beta = 5 ) ), sql::integrity_constraint_violation );
    }
    catch(const sql::failure &e){
        std::cout << e.what();
        return 1;
    }

    return 0;
}