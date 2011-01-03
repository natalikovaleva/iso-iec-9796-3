Hash::Hash(Hash_Type type, const char * source, size_t length)
{
    setupHasher(type);
    
    __hash_buffer(getSource(), getSourceLength(), data, sizeof(data));
}

Hash::Hash(Hash_Type type, const ZZ_p & source)
{
    setupHasher(type);
    __hash_buffer(getSource(), getSourceLength(), data, sizeof(data));
}

Hash::Hash(Hash_Type type, const ZZ & source)
{
    setupHasher(type);
    __hash_buffer(getSource(), getSourceLength(), data, sizeof(data));
}

