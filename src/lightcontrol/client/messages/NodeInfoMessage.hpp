#include <boost/asio/buffer.hpp>

#include <string>
#include <cstdint>

#include "LightModes.hpp"


namespace LightControl {
	enum class BoardType {
		BOARD_TYPE_SINGLE_CHANNEL,
		BOARD_TYPE_DUAL_CHANNEL,
	};

	enum class LightType {
		LIGHT_TYPE_STRING,
		LIGHT_TYPE_PANEL,
		LIGHT_TYPE_RING,
		LIGHT_TYPE_CNT
	};

	enum class NodeType {
		NODE_TYPE_ROOT,
		NODE_TYPE_MESH,
		NODE_TYPE_FORCE_ROOT,
	};

	class LightControlNode {
	public:
		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer containing data for node.
		 */
		LightControlNode(boost::asio::mutable_buffer& buffer);

		/**
		 * @brief Get expected chucksize per node.
		 */
		static size_t getNodeChunkSize() { return 113; }

		/**
		 * @brief Get string representation of node info.
		 */
		std::string getString();

		/** Variables */
		std::string name;
		double _nodeTime;

		LightMode _localMode;
		bool _isModeGlobal;

		uint8_t _layer;
		uint8_t _macAddr[6];
		uint8_t _parentAddr[6];
		
		BoardType _boardType;
		LightType _lightType;
	};

	class NodeInfoMessage {
	public:
		/**
		 * @brief default Constructor
		 */
		NodeInfoMessage() {};

		/**
		 * @brief Constructor
		 *
		 * @param[in] buffer containing message data.
		 */
		NodeInfoMessage(boost::asio::mutable_buffer& buffer);

		/** Variables */
		std::vector<LightControlNode> nodes;
	};
}