#pragma once

/**
  classes copied from http://pqxx.org/devprojects/libpqxx/doc/3.1/html/Reference/a00032.html
*/

#include <sqlpp11/exception.h>
#include <string>
#include "visibility.h"

namespace sqlpp
{
namespace postgresql{
/**
 * @addtogroup exception Exception classes
 *
 * These exception classes follow, roughly, the two-level hierarchy defined by
 * the PostgreSQL error codes (see Appendix A of the PostgreSQL documentation
 * corresponding to your server version).  The hierarchy given here is, as yet,
 * not a complete mirror of the error codes.  There are some other differences
 * as well, e.g. the error code statement_completion_unknown has a separate
 * status in libpqxx as in_doubt_error, and too_many_connections is classified
 * as a broken_connection rather than a subtype of insufficient_resources.
 *
 * @see http://www.postgresql.org/docs/8.1/interactive/errcodes-appendix.html
 *
 * @{
 */

/// Run-time failure encountered by sqlpp::postgresql connector, similar to std::runtime_error
class DLL_PUBLIC failure : public ::sqlpp::exception
{
  virtual const std::exception &base() const throw () { return *this; }
public:
  explicit failure(const std::string &);
};


/// Exception class for lost or failed backend connection.
/**
 * @warning When this happens on Unix-like systems, you may also get a SIGPIPE
 * signal.  That signal aborts the program by default, so if you wish to be able
 * to continue after a connection breaks, be sure to disarm this signal.
 *
 * If you're working on a Unix-like system, see the manual page for
 * @c signal (2) on how to deal with SIGPIPE.  The easiest way to make this
 * signal harmless is to make your program ignore it:
 *
 * @code
 * #include <signal.h>
 *
 * int main()
 * {
 *   signal(SIGPIPE, SIG_IGN);
 *   // ...
 * @endcode
 */
class DLL_PUBLIC broken_connection : public failure
{
public:
  broken_connection();
  explicit broken_connection(const std::string &);
};


/// Exception class for failed queries.
/** Carries a copy of the failed query in addition to a regular error message */
class DLL_PUBLIC sql_error : public failure
{
  std::string m_Q;

public:
  sql_error();
  explicit sql_error(const std::string &);
  sql_error(const std::string &, const std::string &Q);
  virtual ~sql_error() throw ();

  /// The query whose execution triggered the exception
  const std::string & query() const throw ();
};


// TODO: should this be called statement_completion_unknown!?
/// "Help, I don't know whether transaction was committed successfully!"
/** Exception that might be thrown in rare cases where the connection to the
 * database is lost while finishing a database transaction, and there's no way
 * of telling whether it was actually executed by the backend.  In this case
 * the database is left in an indeterminate (but consistent) state, and only
 * manual inspection will tell which is the case.
 */
class DLL_PUBLIC in_doubt_error : public failure
{
public:
  explicit in_doubt_error(const std::string &);
};


/// Database feature not supported in current setup
class DLL_PUBLIC feature_not_supported : public sql_error
{
public:
  explicit feature_not_supported(const std::string &err) : sql_error(err) {}
  feature_not_supported(const std::string &err, const std::string &Q) :
    sql_error(err,Q) {}
};

/// Error in data provided to SQL statement
class DLL_PUBLIC data_exception : public sql_error
{
public:
  explicit data_exception(const std::string &err) : sql_error(err) {}
  data_exception(const std::string &err, const std::string &Q) :
    sql_error(err,Q) {}
};

class DLL_PUBLIC integrity_constraint_violation : public sql_error
{
public:
  explicit integrity_constraint_violation(const std::string &err) :
    sql_error(err) {}
  integrity_constraint_violation(const std::string &err,
    const std::string &Q) :
    sql_error(err, Q) {}
};

class DLL_PUBLIC restrict_violation :
  public integrity_constraint_violation
{
public:
  explicit restrict_violation(const std::string &err) :
    integrity_constraint_violation(err) {}
  restrict_violation(const std::string &err,
    const std::string &Q) :
    integrity_constraint_violation(err, Q) {}
};

class DLL_PUBLIC not_null_violation :
  public integrity_constraint_violation
{
public:
  explicit not_null_violation(const std::string &err) :
    integrity_constraint_violation(err) {}
  not_null_violation(const std::string &err,
    const std::string &Q) :
    integrity_constraint_violation(err, Q) {}
};

class DLL_PUBLIC foreign_key_violation :
  public integrity_constraint_violation
{
public:
  explicit foreign_key_violation(const std::string &err) :
    integrity_constraint_violation(err) {}
  foreign_key_violation(const std::string &err,
    const std::string &Q) :
    integrity_constraint_violation(err, Q) {}
};

class DLL_PUBLIC unique_violation :
  public integrity_constraint_violation
{
public:
  explicit unique_violation(const std::string &err) :
    integrity_constraint_violation(err) {}
  unique_violation(const std::string &err,
    const std::string &Q) :
    integrity_constraint_violation(err, Q) {}
};

class DLL_PUBLIC check_violation :
  public integrity_constraint_violation
{
public:
  explicit check_violation(const std::string &err) :
    integrity_constraint_violation(err) {}
  check_violation(const std::string &err,
    const std::string &Q) :
    integrity_constraint_violation(err, Q) {}
};

class DLL_PUBLIC invalid_cursor_state : public sql_error
{
public:
  explicit invalid_cursor_state(const std::string &err) : sql_error(err) {}
  invalid_cursor_state(const std::string &err, const std::string &Q) :
    sql_error(err,Q) {}
};

class DLL_PUBLIC invalid_sql_statement_name : public sql_error
{
public:
  explicit invalid_sql_statement_name(const std::string &err) :
    sql_error(err) {}
  invalid_sql_statement_name(const std::string &err, const std::string &Q) :
    sql_error(err,Q) {}
};

class DLL_PUBLIC invalid_cursor_name : public sql_error
{
public:
  explicit invalid_cursor_name(const std::string &err) : sql_error(err) {}
  invalid_cursor_name(const std::string &err, const std::string &Q) :
    sql_error(err,Q) {}
};

class DLL_PUBLIC syntax_error : public sql_error
{
public:
  /// Approximate position in string where error occurred, or -1 if unknown.
  const int error_position;

  explicit syntax_error(const std::string &err, int pos=-1) :
    sql_error(err), error_position(pos) {}
  syntax_error(const std::string &err, const std::string &Q, int pos=-1) :
    sql_error(err,Q), error_position(pos) {}
};

class DLL_PUBLIC undefined_column : public syntax_error
{
public:
  explicit undefined_column(const std::string &err) : syntax_error(err) {}
  undefined_column(const std::string &err, const std::string &Q) :
    syntax_error(err, Q) {}
};

class DLL_PUBLIC undefined_function : public syntax_error
{
public:
  explicit undefined_function(const std::string &err) : syntax_error(err) {}
  undefined_function(const std::string &err, const std::string &Q) :
    syntax_error(err, Q) {}
};

class DLL_PUBLIC undefined_table : public syntax_error
{
public:
  explicit undefined_table(const std::string &err) : syntax_error(err) {}
  undefined_table(const std::string &err, const std::string &Q) :
    syntax_error(err, Q) {}
};

class DLL_PUBLIC insufficient_privilege : public sql_error
{
public:
  explicit insufficient_privilege(const std::string &err) : sql_error(err) {}
  insufficient_privilege(const std::string &err, const std::string &Q) :
    sql_error(err,Q) {}
};

/// Resource shortage on the server
class DLL_PUBLIC insufficient_resources : public sql_error
{
public:
  explicit insufficient_resources(const std::string &err) : sql_error(err) {}
  insufficient_resources(const std::string &err, const std::string &Q) :
    sql_error(err,Q) {}
};

class DLL_PUBLIC disk_full : public insufficient_resources
{
public:
  explicit disk_full(const std::string &err) : insufficient_resources(err) {}
  disk_full(const std::string &err, const std::string &Q) :
    insufficient_resources(err,Q) {}
};

class DLL_PUBLIC out_of_memory : public insufficient_resources
{
public:
  explicit out_of_memory(const std::string &err) :
    insufficient_resources(err) {}
  out_of_memory(const std::string &err, const std::string &Q) :
    insufficient_resources(err,Q) {}
};

class DLL_PUBLIC too_many_connections : public broken_connection
{
public:
  explicit too_many_connections(const std::string &err) :
    broken_connection(err) {}
};

/// PL/pgSQL error
/** Exceptions derived from this class are errors from PL/pgSQL procedures.*/
class DLL_PUBLIC plpgsql_error : public sql_error
{
public:
  explicit plpgsql_error(const std::string &err) :
    sql_error(err) {}
  plpgsql_error(const std::string &err, const std::string &Q) :
    sql_error(err, Q) {}
};

/// Exception raised in PL/pgSQL procedure
class DLL_PUBLIC plpgsql_raise : public plpgsql_error
{
public:
  explicit plpgsql_raise(const std::string &err) :
    plpgsql_error(err) {}
  plpgsql_raise(const std::string &err, const std::string &Q) :
    plpgsql_error(err, Q) {}
};

class DLL_PUBLIC plpgsql_no_data_found : public plpgsql_error
{
public:
  explicit plpgsql_no_data_found(const std::string &err) :
    plpgsql_error(err) {}
  plpgsql_no_data_found(const std::string &err, const std::string &Q) :
    plpgsql_error(err, Q) {}
};

class DLL_PUBLIC plpgsql_too_many_rows : public plpgsql_error
{
public:
  explicit plpgsql_too_many_rows(const std::string &err) :
    plpgsql_error(err) {}
  plpgsql_too_many_rows(const std::string &err, const std::string &Q) :
    plpgsql_error(err, Q) {}
};
}
}