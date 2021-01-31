// Radio Settings
#define MYID 2
#define HUBID 1
#define NETWORKID 1
#define FREQUENCY RF69_915MHZ

#define ENCRYPT		false // Set to "true" to use encryption
#define ENCRYPTKEY	"TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes
#define RETRY_COUNT	5
#define RETRY_WAIT	10

#define ZONE_ID 1

// Command Types
enum command : uint8_t
{
	curr_temp,
	target_temp,
	vent_state
};

// Payload Struct Defintion
typedef struct
{
	uint8_t zoneID;	//store this nodeId
	command type;	//uptime in ms
	uint8_t data;	//temperature maybe?
} Payload;
