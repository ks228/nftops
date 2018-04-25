sudo nft add table ip6 natcap
sudo nft add map ip6 natcap simplemap {type ipv6_addr :  verdict\;}
sudo nft add map ip6 natcap natcap_vmap {type ipv6_addr . ipv6_addr :  verdict\;}
sudo nft list table ip6 natcap
