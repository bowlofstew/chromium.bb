/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @constructor
 * @extends {WebInspector.CountersGraph}
 * @implements {WebInspector.TimelineModeView}
 * @param {!WebInspector.TimelineModeViewDelegate} delegate
 * @param {!WebInspector.TimelineModel} model
 */
WebInspector.MemoryCountersGraph = function(delegate, model)
{
    WebInspector.CountersGraph.call(this, delegate, model);
    this._countersByName = {};
    this._countersByName["documents"] = this.createCounter(WebInspector.UIString("Documents"), WebInspector.UIString("Documents: %d"), "#d00");
    this._countersByName["nodes"] = this.createCounter(WebInspector.UIString("Nodes"), WebInspector.UIString("Nodes: %d"), "#0a0");
    this._countersByName["jsEventListeners"] = this.createCounter(WebInspector.UIString("Listeners"), WebInspector.UIString("Listeners: %d"), "#00d");
    if (WebInspector.experimentsSettings.gpuTimeline.isEnabled())
        this._countersByName["gpuMemoryUsedKB"] = this.createCounter(WebInspector.UIString("GPU Memory"), WebInspector.UIString("GPU Memory [KB]: %d"), "#c0c");
}

WebInspector.MemoryCountersGraph.prototype = {
    /**
     * @param {!WebInspector.TimelineModel.Record} record
     */
    addRecord: function(record)
    {
        /**
         * @param {!WebInspector.TimelineModel.Record} record
         * @this {!WebInspector.MemoryCountersGraph}
         */
        function addStatistics(record)
        {
            var counters = record.counters;
            if (!counters)
                return;

            for (var name in counters) {
                var counter = this._countersByName[name];
                if (counter)
                    counter.appendSample(record.endTime || record.startTime, counters[name]);
            }
        }
        WebInspector.TimelineModel.forAllRecords([record], null, addStatistics.bind(this));
        this.scheduleRefresh();
    },

    __proto__: WebInspector.CountersGraph.prototype
}