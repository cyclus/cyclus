"""Main Cyclus CLI entry point."""
from __future__ import unicode_literals, print_function
import os
import atexit
from argparse import ArgumentParser, Action

from cyclus.jsoncpp import CustomWriter
from cyclus.lib import (DynamicModule, Env, version, load_string_from_file,
    Recorder, Timer, Context, set_warn_limit, discover_specs, XMLParser,
    discover_specs_in_cyclus_path, discover_metadata_in_cyclus_path, Logger,
    set_warn_limit, set_warn_as_error, xml_to_json, json_to_xml,
    Hdf5Back, SqliteBack, InfileTree, SimInit, XMLFileLoader, XMLFlatLoader)


# ensure that Cyclus dynamic modules are closed when Python exits.
_DYNAMIC_MODULE = DynamicModule()
atexit.register(_DYNAMIC_MODULE.close_all)


LOGO = """              :
          .CL:CC CC             _Q     _Q  _Q_Q    _Q    _Q              _Q
        CC;CCCCCCCC:C;         /_\\)   /_\\)/_/\\\\)  /_\\)  /_\\)            /_\\)
        CCCCCCCCCCCCCl       __O|/O___O|/O_OO|/O__O|/O__O|/O____________O|/O__
     CCCCCCf     iCCCLCC     /////////////////////////////////////////////////
     iCCCt  ;;;;;.  CCCC
    CCCC  ;;;;;;;;;. CClL.                          c
   CCCC ,;;       ;;: CCCC  ;                   : CCCCi
    CCC ;;         ;;  CC   ;;:                CCC`   `C;
  lCCC ;;              CCCC  ;;;:             :CC .;;. C;   ;    :   ;  :;;
  CCCC ;.              CCCC    ;;;,           CC ;    ; Ci  ;    :   ;  :  ;
   iCC :;               CC       ;;;,        ;C ;       CC  ;    :   ; .
  CCCi ;;               CCC        ;;;.      .C ;       tf  ;    :   ;  ;.
  CCC  ;;               CCC          ;;;;;;; fC :       lC  ;    :   ;    ;:
   iCf ;;               CC         :;;:      tC ;       CC  ;    :   ;     ;
  fCCC :;              LCCf      ;;;:         LC :.  ,: C   ;    ;   ; ;   ;
  CCCC  ;;             CCCC    ;;;:           CCi `;;` CC.  ;;;; :;.;.  ; ,;
    CCl ;;             CC    ;;;;              CCC    CCL
   tCCC  ;;        ;; CCCL  ;;;                  tCCCCC.
    CCCC  ;;     :;; CCCCf  ;                     ,L
     lCCC   ;;;;;;  CCCL
     CCCCCC  :;;  fCCCCC
      . CCCC     CCCC .
       .CCCCCCCCCCCCCi
          iCCCCCLCf
           .  C. ,
              :  """


def set_schema_path(ns):
    """Sets the schema path on the namespace."""
    if ns.flat_schema:
        path = Env.rng_schema(True)
    elif ns.schema_path is not None:
        path = ns.schema_path
    else:
        path = Env.rng_schema(False)
    ns.schema_path = path


class ZeroArgAction(Action):
    """An action with no arguments."""

    def __init__(self, option_strings, dest, nargs=0, **kwargs):
        super(ZeroArgAction, self).__init__(option_strings, dest, nargs=nargs,
                                            **kwargs)

class CyclusVersion(ZeroArgAction):
    """Displays the cyclus version"""

    def __call__(self, parser, namespace, values, option_string=None):
        print(version(), end="")


class Restart(Action):
    """Restart action for cyclus. Not supported."""

    def __call__(self, parser, namespace, values, option_string=None):
        raise RuntimeError("Restart is experimental, not supported!")


class Schema(ZeroArgAction):
    """Displays cyclus schema"""

    def __call__(self, parser, ns, values, option_string=None):
        set_schema_path(ns)
        schema = load_string_from_file(ns.schema_path)
        print(schema)


def make_agent_ctx(spec):
    """Contsructs and agent and returns the agent and the context."""
    set_warn_limit(0)
    rec = Recorder()
    ti = Timer()
    ctx = Context(ti, rec)
    agent = DynamicModule.make(ctx, spec)
    return agent, ctx


class AgentSchema(Action):
    """Displays an agent schema"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_schema = values
        agent, ctx = make_agent_ctx(ns.agent_schema)
        print(agent.schema.rstrip())
        ctx.del_agent(agent)


class AgentVersion(Action):
    """Displays an agent version"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_version = values
        agent, ctx = make_agent_ctx(ns.agent_version)
        print(agent.version)
        ctx.del_agent(agent)


class AgentAnnotations(Action):
    """Displays an agent annotations"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_annotations = values
        agent, ctx = make_agent_ctx(ns.agent_annotations)
        anno = agent.annotations
        writer = CustomWriter("{", "}", "[", "]", ": ", ", ", " ", 80)
        s = writer.write(anno)
        print(s.rstrip())
        ctx.del_agent(agent)


class AgentListing(Action):
    """Displays an agent listing"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.agent_listing = values
        path, library = ns.agent_listing.split(':')
        specs = discover_specs(path, library)
        s = '\n'.join(sorted(specs))
        print(s)


class AllAgentListing(ZeroArgAction):
    """Displays all known cyclus agents"""

    def __call__(self, parser, ns, values, option_string=None):
        specs = discover_specs_in_cyclus_path()
        s = '\n'.join(sorted(specs))
        print(s)


class Metadata(ZeroArgAction):
    """Displays all known cyclus agents"""

    def __call__(self, parser, ns, values, option_string=None):
        md = discover_metadata_in_cyclus_path()
        writer = CustomWriter("{", "}", "[", "]", ": ", ", ", " ", 80)
        s = writer.write(md)
        print(s.rstrip())


class NoAgent(ZeroArgAction):
    """Disables agent logging"""

    def __call__(self, parser, ns, values, option_string=None):
        Logger().no_agent = True


class NoMem(ZeroArgAction):
    """Disables mem logging"""

    def __call__(self, parser, ns, values, option_string=None):
        Logger().no_mem = True


class Verbosity(Action):
    """Sets versbosity level"""

    def __call__(self, parser, ns, values, option_string=None):
        logger = Logger()
        try:
            ns.verbosity = int(values)
        except ValueError:
            ns.verbosity = logger.to_log_level(values)
        logger.report_level = ns.verbosity


class WarnLimit(Action):
    """Sets warning limit"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.warn_limit = int(values)
        set_warn_limit(ns.warn_limit)


class WarnAsError(ZeroArgAction):
    """Sets warning to be errors"""

    def __call__(self, parser, ns, values, option_string=None):
        set_warn_as_error(True)


class CyclusPath(ZeroArgAction):
    """Prints the Cyclus Path"""

    def __call__(self, parser, ns, values, option_string=None):
        cp = Env().cyclus_path
        s = os.pathsep.join(cp)
        print(s)


class IncludePath(ZeroArgAction):
    """Prints the Cyclus Include Path"""

    def __call__(self, parser, ns, values, option_string=None):
        s = os.path.join(Env().install_path, "include", "cyclus")
        print(s)


class InstallPath(ZeroArgAction):
    """Prints the Cyclus Install Path"""

    def __call__(self, parser, ns, values, option_string=None):
        s = Env().install_path
        print(s)


class CMakeModulePath(ZeroArgAction):
    """Prints the Cyclus CMake Module Path"""

    def __call__(self, parser, ns, values, option_string=None):
        s = os.path.join(Env().install_path, "share", "cyclus", "cmake")
        print(s)


class BuildPath(ZeroArgAction):
    """Prints the Cyclus Build Path"""

    def __call__(self, parser, ns, values, option_string=None):
        s = Env().build_path
        print(s)


class RngSchema(ZeroArgAction):
    """Displays path to the rng schema"""

    def __call__(self, parser, ns, values, option_string=None):
        set_schema_path(ns)
        print(ns.schema_path)


class NucData(ZeroArgAction):
    """Prints the Nuclear data Path"""

    def __call__(self, parser, ns, values, option_string=None):
        s = Env().nuc_data
        print(s)


class JsonToXml(Action):
    """converts JSON to XML"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.json_to_xml = values
        with open(ns.json_to_xml, 'r') as f:
            s = f.read()
        t = json_to_xml(s)
        print(t.rstrip())


class XmlToJson(Action):
    """converts XML to JSON"""

    def __call__(self, parser, ns, values, option_string=None):
        ns.xml_to_json = values
        with open(ns.xml_to_json, 'r') as f:
            s = f.read()
        t = xml_to_json(s)
        print(t.rstrip())


def make_parser():
    """Makes the Cyclus CLI parser."""
    p = ArgumentParser("cyclus", description="Cyclus command line "
                                             "(from Python)")
    p.add_argument('-V', action=CyclusVersion,
                   help='print cyclus core and dependency versions')
    p.add_argument('--restart', action=Restart,
                   help='restart from the specified simulation snapshot, '
                        'not supported.')
    p.add_argument('--schema', action=Schema,
                   help='dump the cyclus master schema including all '
                        'installed module schemas')
    p.add_argument('--agent-schema', action=AgentSchema,
                   dest='agent_schema',
                   help='dump the schema for the named agent')
    p.add_argument('--agent-version', action=AgentVersion,
                   dest='agent_version',
                   help='dump the version for the named agent')
    p.add_argument('--schema-path', dest='schema_path', default=None,
                   help='manually specify the path to the cyclus master schema')
    p.add_argument('--flat-schema', action='store_true', default=False,
                   dest='flat_schema',
                   help='use the flat master simulation schema')
    p.add_argument('--agent-annotations', action=AgentAnnotations,
                   dest='agent_annotations',
                   help='dump the annotations for the named agent')
    p.add_argument('-l', '--agent-listing', action=AgentListing,
                   dest='agent_listing',
                   help='dump the agents in a library')
    p.add_argument('-a', '--all-agent-listing', action=AllAgentListing,
                   help='dump all the agents cyclus knows about')
    p.add_argument('-m', '--metadata', action=Metadata,
                   help='dump metadata for all the agents cyclus knows about')
    p.add_argument('--no-agent', action=NoAgent,
                   help='only print log entries from cyclus core code')
    p.add_argument('--no-mem', action=NoMem,
                   help='exclude memory log statement from logger output')
    p.add_argument('-v', '--verb', action=Verbosity, dest='verbosity',
                   help='log verbosity. integer from 0 (quiet) to 11 (verbose)')
    p.add_argument('-o', '--output-path', dest='output_path',
                   default='cyclus.sqlite', help='output path')
    p.add_argument('--input-file', dest='input_file', default=None,
                   help="path to input file")
    p.add_argument('--warn-limit', dest='warn_limit', action=WarnLimit,
                   help='number of warnings to issue per kind, defaults to 42')
    p.add_argument('--warn-as-error', action=WarnAsError,
                   help='throw errors when warnings are issued')
    p.add_argument('-p', '--path', action=CyclusPath,
                   help='print the CYCLUS_PATH')
    p.add_argument('--include', action=IncludePath,
                   help='print the cyclus include directory')
    p.add_argument('--install-path', action=InstallPath,
                   help='print the cyclus install directory')
    p.add_argument('--cmake-module-path', action=CMakeModulePath,
                   help='print the cyclus CMake module path')
    p.add_argument('--build-path', action=BuildPath,
                   help='print the cyclus build directory')
    p.add_argument('--rng-schema', action=RngSchema,
                   help='print the path to cyclus.rng.in')
    p.add_argument('--nuc-data', action=NucData,
                   help='print the path to cyclus_nuc_data.h5')
    p.add_argument('--json-to-xml', action=JsonToXml,
                   dest='json_to_xml', default=None,
                   help='*.json input file')
    p.add_argument('--xml-to-json', action=XmlToJson,
                   dest='xml_to_json', default=None,
                   help='*.xml input file')
    p.add_argument('input_file', nargs='?',
                   help='path to input file')
    return p


def run_simulation(ns):
    """Runs the simulation when we recieve an input file."""
    Env.set_nuc_data_path()
    print(LOGO)
    rec = Recorder()
    # setup database backend
    _, ext = os.path.splitext(ns.output_path)
    if ext == '.h5':
        backend = Hdf5Back(ns.output_path)
    elif ext == '.sqlite':
        backend = SqliteBack(ns.output_path)
    else:
        raise RuntimeError('Backend extension type not recognised, ' +
                           ns.output_path)
    rec.register_backend(backend)
    # find schema type
    parser = XMLParser(filename=ns.input_file)
    tree = InfileTree(parser)
    schema_type = tree.optional_query("/simulation/schematype", "")
    if schema_type == "flat" and not ns.flat_schema:
        print("flat schema tag detected - switching to flat input schema",
              file=sys.stderr)
        ns.flat_schema = True
    set_schema_path(ns)
    # Load input file and initialize simulation
    if ns.flat_schema:
        loader = XMLFlatLoader(rec, backend, ns.schema_path, ns.input_file)
    else:
        loader = XMLFileLoader(rec, backend, ns.schema_path, ns.input_file)
    loader.load_sim()
    si = SimInit(rec, backend)
    # run simulation and report back
    si.timer.run_sim()
    rec.flush()
    msg = ("Status: Cyclus run successful!\n"
           "Output location: {0}\n"
           "Simulation ID: {1}").format(ns.output_path,
                                        si.context.sim_id)
    print(msg)



def main(args=None):
    """Main function for Cyclus CLI"""
    Env.set_nuc_data_path()
    p = make_parser()
    ns = p.parse_args(args=args)
    if ns.input_file is not None:
        run_simulation(ns)


if __name__ == '__main__':
    main()