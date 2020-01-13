"""Root metrics for Cycamore"""
from cymetric import root_metric


#root metrics that come with the archetypes within the cycamore module
agentstate_cycamore_enrichmentinfo = root_metric(
    name='AgentState_cycamore_EnrichmentInfo')
agentstate_cycamore_fuelfabinfo = root_metric(
    name='AgentState_cycamore_FuelFabInfo')
agentstate_cycamore_reactorinfo = root_metric(
    name='AgentState_cycamore_ReactorInfo')
agentstate_cycamore_separationsinfo = root_metric(
    name='AgentState_cycamore_SeparationsInfo')
agentstate_cycamore_sinkinfo = root_metric(
    name='AgentState_cycamore_SinkInfo')
agentstate_cycamore_sourceinfo = root_metric(
    name='AgentState_cycamore_SourceInfo')
enrichments = root_metric(name='Enrichments')
reactor_events = root_metric(name='ReactorEvents')

