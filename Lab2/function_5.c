float pole_walca(float promien, float wysokosc) {
    float pole_podstawy = 3.14159265358979323846 * promien * promien; 
    float pole_boczne = 2.0 * 3.14159265358979323846 * promien * wysokosc; 
    return 2.0 * pole_podstawy + pole_boczne;
}
