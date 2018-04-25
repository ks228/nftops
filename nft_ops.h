/*! \file nft_ops.h 
* \brief nftables interface
*/
#define NFT_MAP_ADD_ELEM 0
#define NFT_MAP_DEL_ELEM 1
#define NFT_VMAP_ADD_ELEM 0
#define NFT_VMAP_DEL_ELEM 1
/*
* \brief This function will add or delete elements from the nft map.
* @action 0=ADD, 1=DELETE
* @key Client_ip6addr
* @val Ephemeral_ip6addr
* @table The nft table to modify
* @map The map in the nft table to add element to

**/
int nft_simple_vmap_elem_do(int action, const char* key,
			const char* table,
			const char* map,
			int verdict);
/*
* \brief This function will add or delete elements from the nft vmap or verdict map. NFT vmap functionaity is used to do filtering.
* @action 0=ADD, 1=DELETE
* @cip Client_ip6addr
* @eip Ephemeral_ip6addr
* @table The nft table to modify
* @vmap The vmap in the nft table to add element to
* @verdict ACCEPT | DROP
**/
int nft_concat_vmap_elem_do(int action, const char* cip,
			const char* eip,
			const char* table,
			const char* vmap,
			int verdict);

