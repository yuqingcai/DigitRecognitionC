
int int32_swap(int v) 
{
    int n = 0;

    n |= ((v & 0xFF000000) >> 24);
    n |= ((v & 0x00FF0000) >> 8);
    n |= ((v & 0x0000FF00) << 8);
    n |= ((v & 0x000000FF) << 24);

    return n;
}