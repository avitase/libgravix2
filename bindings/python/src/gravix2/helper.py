import ctypes
from ctypes import c_double


class Helper:
    def __init__(self, *, lib: ctypes.CDLL) -> None:
        get_lat = lib.lat
        get_lat.argtypes = [c_double]
        get_lat.restype = c_double
        self.get_lat = get_lat

        get_lon = lib.lon
        get_lon.argtypes = [c_double, c_double]
        get_lon.restype = c_double
        self.get_lon = get_lon

        get_vlat = lib.vlat
        get_vlat.argtypes = [c_double, c_double, c_double, c_double, c_double]
        get_vlat.restype = c_double
        self.get_vlat = get_vlat

        get_vlon = lib.vlon
        get_vlon.argtypes = [c_double, c_double, c_double, c_double]
        get_vlon.restype = c_double
        self.get_vlon = get_vlon

        get_vesc = lib.v_esc
        get_vesc.argtypes = None
        get_vesc.restype = c_double
        self.get_vesc = get_vesc

        get_orb_period = lib.orb_period
        get_orb_period.argtypes = [c_double, c_double]
        get_orb_period.restype = c_double
        self.get_orb_period = get_orb_period
