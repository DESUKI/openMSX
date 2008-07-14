// $Id$

#ifndef PLUGGINGCONTROLLER_HH
#define PLUGGINGCONTROLLER_HH

#include "noncopyable.hh"
#include <vector>
#include <string>
#include <memory>

namespace openmsx {

class MSXMotherBoard;
class Connector;
class Pluggable;
class CliComm;
class PlugCmd;
class UnplugCmd;
class PluggableInfo;
class ConnectorInfo;
class ConnectionClassInfo;

/**
 * Central administration of Connectors and Pluggables.
 */
class PluggingController : private noncopyable
{
public:
	explicit PluggingController(MSXMotherBoard& motherBoard);
	~PluggingController();

	/** Connectors must be (un)registered
	  */
	void registerConnector(Connector& connector);
	void unregisterConnector(Connector& connector);

	/**
	 * Add a Pluggable to the registry.
	 * PluggingController has ownership of all registered Pluggables.
	 */
	void registerPluggable(Pluggable* pluggable);

	/**
	 * Removes a Pluggable from the registry.
	 * If you attempt to unregister a Pluggable that is not in the registry,
	 * nothing happens.
	 */
	void unregisterPluggable(Pluggable* pluggable);

	template<typename Archive>
	void serialize(Archive& ar, unsigned version);

private:
	Connector* getConnector(const std::string& name);
	Pluggable* getPluggable(const std::string& name);

	typedef std::vector<Connector*> Connectors;
	Connectors connectors;
	typedef std::vector<Pluggable*> Pluggables;
	Pluggables pluggables;

	friend class PlugCmd;
	friend class UnplugCmd;
	friend class PluggableInfo;
	friend class ConnectorInfo;
	friend class ConnectionClassInfo;
	const std::auto_ptr<PlugCmd> plugCmd;
	const std::auto_ptr<UnplugCmd> unplugCmd;
	const std::auto_ptr<PluggableInfo> pluggableInfo;
	const std::auto_ptr<ConnectorInfo> connectorInfo;
	const std::auto_ptr<ConnectionClassInfo> connectionClassInfo;

	CliComm& cliComm;
};

} // namespace openmsx

#endif
