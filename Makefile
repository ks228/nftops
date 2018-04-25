vmake: nft_simplemap_elem_add.c nft_ops.h
	     gcc nft_simplemap_elem_add.c nft_ops.h -o nft_map_without_concat -lmnl -lnftnl 
vmakeconcat: nft_concatmap_elem_add.c nft_ops.h
	     gcc nft_concatmap_elem_add.c nft_ops.h -o nft_map_with_concat -lmnl -lnftnl
all: vmake vmakeconcat
