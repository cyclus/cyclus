"""Python wrapper for nucname library."""
from __future__ import unicode_literals, division, print_function

# Cython imports
from libcpp.map cimport map
from libcpp.set cimport set as cpp_set
from cython.operator cimport dereference as deref
from cython.operator cimport preincrement as inc
from libcpp.string cimport string as std_string

from cyclus cimport cpp_cyclus as cpp_nucname


class NucTypeError(Exception):
    def __init__(self, nuc=None):
        self.nuc = nuc

    def __str__(self):
        msg = "Nuclide type not an int or str"
        if self.nuc is not None:
            msg += ": " + repr(self.nuc)
        return msg


#
# Is Nuclide and Is Element Functions
#

def isnuclide(nuc):
    """Test if nuc is a valid nuclide.

    Parameters
    ----------
    nuc : int or str
        Input nuclide(s).

    Returns
    -------
    flag : bool

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        flag = cpp_nucname.isnuclide(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        flag = cpp_nucname.isnuclide(<int> nuc)
    else:
        raise NucTypeError(nuc)

    return flag

def iselement(nuc):
    """Test if nuc is a valid element.

    Parameters
    ----------
    nuc : int or str
        Input element.

    Returns
    -------
    flag : bool

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        flag = cpp_nucname.iselement(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        flag = cpp_nucname.iselement(<int> nuc)
    else:
        raise NucTypeError(nuc)

    return flag

def id(nuc):
    """Converts a nuclide to its identifier form (952420000).

    If the input nuclide is in id form already, then this is function does no
    work. For all other formats, the id() function provides a best-guess based
    on a heirarchy of other formats that is used to resolve ambiguities between
    naming conventions. For integer input the form resolution order is:

        - id
        - zz (elemental z-num only given)
        - zzaaam
        - cinder (aaazzzm)
        - mcnp
        - zzaaa

    For string (or char *) input the form resolution order is as follows:

        - ZZ-LL-AAAM
        - Integer form in a string representation, uses interger resolution
        - NIST
        - name form
        - Serpent
        - LL (element symbol)

    For well-defined situations where you know ahead of time what format the
    nuclide is in, you should use the various form_to_id() functions, rather
    than the id() function which is meant to resolve possibly ambiquous cases.

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide id.

    """
    if isinstance(nuc, basestring):
        nuc = nuc.encode()
        newnuc = cpp_nucname.id(<char *> nuc)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def name(nuc):
    """Converts a nuclide to its name form ('Am242M'). The name() function
    first converts functions to id form using the id() function. Thus the
    form order resolution for id() also applies to here.

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : str
        Output nuclide in name form.

    """
    cdef std_string newnuc
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.name(<char *> nuc_bytes)
    elif isinstance(nuc, int):
        newnuc = cpp_nucname.name(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return bytes(<char *> newnuc.c_str()).decode()


def znum(nuc):
    """Retrieves a nuclide's charge number (95).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    z : int
        The number of protons in the nucleus.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        z = cpp_nucname.znum(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        z = cpp_nucname.znum(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return z


def anum(nuc):
    """Retrieves a nuclide's nucleon number (95).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    a : int
        The number of protons and neutrons in the nucleus.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        a = cpp_nucname.anum(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        a = cpp_nucname.anum(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return a


def snum(nuc):
    """Retrieves a nuclide's excitation number (95).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    s : int
        The excitation level the nucleus.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        s = cpp_nucname.snum(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        s = cpp_nucname.snum(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return s


def zzaaam(nuc):
    """Converts a nuclide to its zzaaam form (952420).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in zzaaam form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.zzaaam(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.zzaaam(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def zzaaam_to_id(nuc):
    """Converts a nuclide directly from ZZAAAM form (952420) to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in ZZAAAM form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.zzaaam_to_id(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.zzaaam_to_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def zzzaaa(nuc):
    """Converts a nuclide to its zzzaaa form (95242).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in zzzaaa form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.zzzaaa(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.zzzaaa(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def zzzaaa_to_id(nuc):
    """Converts a nuclide directly from ZZZAAA form (95242) to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in ZZZAAA form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        newnuc = cpp_nucname.zzzaaa_to_id(<char *> nuc)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.zzzaaa_to_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def mcnp(nuc):
    """Converts a nuclide to its MCNP form (92636).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in MCNP form.

    Notes
    -----
    Most metastables in this form add 300 + 100*m where
    m is the isomeric state (U-236m = 92636).  However,
    MCNP special cases Am-242 and Am-242m by switching
    the meaning. Thus Am-242m = 95242 and Am-242 = 95642.

    """

    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.mcnp(<char *> nuc_bytes)
    elif isinstance(nuc, int):
        newnuc = cpp_nucname.mcnp(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def mcnp_to_id(nuc):
    """Converts a nuclide directly from MCNP form (92636) to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in MCNP form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.mcnp_to_id(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.mcnp_to_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def fluka(nuc):
    """Converts a nuclide to its FLUKA name.

    Parameters
    ----------
    nuc : int
        Input nuclide.

    Returns
    -------
    fluka_name : string
        Output name in FLUKA form.


    """
    fluka_name = cpp_nucname.fluka(nuc)
    return fluka_name.decode()


def fluka_to_id(name):
    """Converts a fluka name to the canonical identifier form.

    Parameters
    ----------
    name : str
        Input name, expectedt to be one FLUKA knows

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    name = name.encode()
    newnuc = cpp_nucname.fluka_to_id(<char *> name)
    return newnuc


def zzllaaam(nuc):
    """Converts a nuclide to its zzllaaam form (95-Am-241m).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : str
        Output nuclide in zzllaaam form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.zzllaaam(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.zzllaaam(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return bytes(newnuc).decode()


def zzllaaam_to_id(nuc):
    """Converts a nuclide directly from ZZLLAAAM form (95-Am-241m) to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in ZZLLAAAM form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.zzllaaam_to_id(<char *> nuc_bytes)
    else:
        raise NucTypeError(nuc)
    return newnuc



def serpent(nuc):
    """Converts a nuclide to its Serepnt form ('Am-242m').

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : str
        Output nuclide in serpent form.

    """
    cdef std_string newnuc

    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.serpent(<char *> nuc_bytes)
    elif isinstance(nuc, int):
        newnuc = cpp_nucname.serpent(<int> nuc)
    else:
        raise NucTypeError(nuc)

    return bytes(<char *> newnuc.c_str()).decode()


def serpent_to_id(nuc):
    """Converts a nuclide directly from Serpent form ('Am-242m') to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in Serpent form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.serpent_to_id(<char *> nuc_bytes)
    #elif isinstance(nuc, int) or isinstance(nuc, long):
    #    newnuc = cpp_nucname.serpent_to_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def nist(nuc):
    """Converts a nuclide to NIST form ('242Am').

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : str
        Output nuclide in nist form.

    """
    cdef std_string newnuc

    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.nist(<char *> nuc_bytes)
    elif isinstance(nuc, int):
        newnuc = cpp_nucname.nist(<int> nuc)
    else:
        raise NucTypeError(nuc)

    return bytes(<char *> newnuc.c_str()).decode()


def nist_to_id(nuc):
    """Converts a nuclide directly from NIST form ('242Am') to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in NIST form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.nist_to_id(<char *> nuc_bytes)
    #elif isinstance(nuc, int) or isinstance(nuc, long):
    #    newnuc = cpp_nucname.nist_to_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def cinder(nuc):
    """Converts a nuclide to its CINDER (aaazzzm) form (2420951).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in CINDER (aaazzzm) form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.cinder(<char *> nuc_bytes)
    elif isinstance(nuc, int):
        newnuc = cpp_nucname.cinder(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def cinder_to_id(nuc):
    """Converts a nuclide directly from Cinder form (2420951) to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in Cinder form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        newnuc = cpp_nucname.cinder_to_id(<char *> nuc)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.cinder_to_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def alara(nuc):
    """Converts a nuclide to its ALARA form ('am:242').

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : str
        Output nuclide in name form.

    """
    cdef std_string newnuc

    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.alara(<char *> nuc_bytes)
    elif isinstance(nuc, int):
        newnuc = cpp_nucname.alara(<int> nuc)
    else:
        raise NucTypeError(nuc)

    return bytes(<char *> newnuc.c_str()).decode()


def alara_to_id(nuc):
    """Converts a nuclide directly from ALARA form ('am:242') to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in ALARA form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.alara_to_id(<char *> nuc_bytes)
    #elif isinstance(nuc, int) or isinstance(nuc, long):
    #    newnuc = cpp_nucname.alara_to_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def sza(nuc):
    """Converts a nuclide to its SZA form (SSSZZZAAA).

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in SZA form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.sza(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.sza(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def sza_to_id(nuc):
    """Converts a nuclide directly from SZA form (SSSZZZAAA) to
    the canonical identifier form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide in SZA form.

    Returns
    -------
    newnuc : int
        Output nuclide in identifier form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.sza_to_id(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.sza_to_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def groundstate(nuc):
    """Converts a nuclide to its Groundstate form.

    Parameters
    ----------
    nuc : int or str
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in Groundstate form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        newnuc = cpp_nucname.groundstate(<char *> nuc_bytes)
    elif isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.groundstate(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc


def state_id_to_id(state):
    """
    Converts a ENSDF state id to a PyNE nuc_id

    Parameters
    ----------
    nuc : int
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in nuc_id form.

    """
    if isinstance(state, int) or isinstance(state, long):
        newnuc = cpp_nucname.state_id_to_id(<int> state)
    else:
        raise NucTypeError(state)
    return newnuc


def id_to_state_id(nuc):
    """
    Converts a ENSDF state id to a PyNE nuc_id

    Parameters
    ----------
    nuc : int
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in nuc_id form.

    """
    if isinstance(nuc, int) or isinstance(nuc, long):
        newnuc = cpp_nucname.id_to_state_id(<int> nuc)
    else:
        raise NucTypeError(nuc)
    return newnuc

def ensdf_to_id(nuc):
    """
    Converts an ENSDF style id to a PyNE nuc_id

    Parameters
    ----------
    nuc : int
        Input nuclide.

    Returns
    -------
    newnuc : int
        Output nuclide in nuc_id form.

    """
    if isinstance(nuc, basestring):
        nuc_bytes = nuc.encode()
        return cpp_nucname.ensdf_to_id(<char *> nuc_bytes)
    else:
        raise NucTypeError(nuc)
