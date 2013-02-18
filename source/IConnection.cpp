#include "IConnection.h"

std::ostream& operator<<(std::ostream & os, const IConnection& connection)
{
	std::ostream::sentry ok(os);

	os << connection.connectionInfo();
	return os;
}
