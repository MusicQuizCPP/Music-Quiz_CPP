#include "NodeInfoMessage.hpp"

#include "common/Log.hpp"

#include <cstring>
#include <sstream>
#include <iomanip>


using namespace LightControl;

NodeInfoMessage::NodeInfoMessage(boost::asio::mutable_buffer& buffer)
{
	if ( buffer.size() < sizeof(uint32_t) ) {
		LOG_ERROR("Received Node Info message with invalid size: " + std::to_string(buffer.size()));
	}

	uint32_t nodeCnt = *static_cast<uint32_t*>(buffer.data());

	buffer += sizeof(nodeCnt);

	size_t expectedSize = nodeCnt * LightControlNode::getNodeChunkSize();
	if ( buffer.size() != expectedSize ) {
		LOG_ERROR("Received Node info message with unexpected size. Received " + std::to_string(buffer.size()) + ". Expected " + std::to_string(expectedSize));
	}

	for ( size_t i = 0; i < nodeCnt; i++ ) {
		nodes.push_back(LightControlNode(buffer));
		buffer += LightControlNode::getNodeChunkSize();
	}
}

LightControlNode::LightControlNode(boost::asio::mutable_buffer& buffer)
{
	memcpy(_macAddr, buffer.data(), sizeof(_macAddr));
	buffer += sizeof(_macAddr);

	memcpy(_parentAddr, buffer.data(), sizeof(_parentAddr));
	buffer += sizeof(_parentAddr);

	_nodeTime = *static_cast<double*>(buffer.data());
	buffer += sizeof(_nodeTime);

	/** Skip mean air time */
	buffer += sizeof(float);

	/** Name */
	uint8_t nameLen = *static_cast<uint8_t*>(buffer.data());
	nameLen = nameLen <= 20 ? nameLen : 20;

	buffer += sizeof(nameLen);
	char nameBuf[21];
	memcpy(nameBuf, buffer.data(), nameLen);
	nameBuf[nameLen] = '\0';
	this->name = std::string(nameBuf);
	buffer += 20;

	_boardType = static_cast<BoardType>(*static_cast<uint8_t*>(buffer.data()));
	buffer += 1;

	/** Skip strobe offset */
	buffer += 4;

	/** Skip left length */
	buffer += 2;

	/** Skip right length */
	buffer += 2;

	_localMode = static_cast<LightMode>(*static_cast<uint8_t*>(buffer.data()));
	buffer += 1;

	_isModeGlobal = *static_cast<uint8_t*>(buffer.data());
	buffer += 1;

	_layer = *static_cast<uint8_t*>(buffer.data());
	buffer += 1;

	/** Skip left panel height */
	buffer += 1;

	/** Skip left panel width */
	buffer += 1;

	/** Skip right panel height */
	buffer += 1;

	/** Skip right panel width */
	buffer += 1;

	/** Skip left ring settings */
	buffer += 20;

	/** Skip right ring settings */
	buffer += 20;

	_lightType = static_cast<LightType>(*static_cast<uint8_t*>(buffer.data()));
}

std::string LightControlNode::getString()
{
	std::ostringstream ss;

	ss << "MAC: " << std::hex << std::uppercase;
	for ( size_t i = 0; i < sizeof(_macAddr); i++ ) {
		ss << std::setw(2) << static_cast<int>(_macAddr[i]);
	}
	ss << std::endl;

	ss << "Parent MAC: " << std::hex << std::uppercase << std::setfill('0');
	for ( size_t i = 0; i < sizeof(_parentAddr); i++ ) {
		ss << std::setw(2) << static_cast<int>(_parentAddr[i]);
	}
	ss << std::endl;
	ss << std::dec;

	ss << "Node Time: " << _nodeTime << std::endl;
	ss << "Name: " << this->name << std::endl;
	ss << "Board Type: " << static_cast<int>(_boardType) << std::endl;
	ss << "Local Mode: " << static_cast<int>(_localMode) << std::endl;
	ss << "Global Mode active: " << _isModeGlobal << std::endl;
	ss << "Layer: " << static_cast<int>(_layer) << std::endl;
	ss << "Light Type: " << static_cast<int>(_lightType) << std::endl;

	return ss.str();
}