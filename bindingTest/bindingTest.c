/*
 * Copyright (C) 2016-2018 "IoT.bzh"
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <time.h>

#define AFB_BINDING_VERSION 3
#include <afb/afb-binding.h>

static afb_event_t event_count, event_date;

static void testCount(afb_req_t request)
{
    static int count = 0;
	int listenersCount = 0;

    json_object *obj = afb_req_json(request);

    count = json_object_get_string_len(obj);

	if(afb_event_is_valid(event_date) && count > 10){
			listenersCount = afb_event_push(event_count, json_object_new_int(count));
	}

    afb_req_success_f(request, json_object_new_int(count), "Length of the sentence is : %d, current listeners : %d", count, listenersCount);

	AFB_API_NOTICE(afbBindingV3root, "Verbosity macro at level notice invoked at count invocation");

}

static void timeDisp(afb_req_t request)
{
	// J-son answer
	json_object *res = json_object_new_object();
	json_object *res_event = json_object_new_object();

	// J-son argument
	json_object *args, *arg;

	time_t secondes;

	struct tm instant;

	int listenersDate = 0;

	// Get the number of second since a specific time
    time(&secondes);

	// Convert the time into the local time zone
	instant=*localtime(&secondes);

	// J-son which gathered arguments in command line
	args = afb_req_json(request);

	// Invalid request
	if (!json_object_object_get_ex(args, "info", &arg)){
		afb_req_fail_f(request, "ERROR", "Invalid request in %s.", json_object_get_string(args));
		return;
	}

	// Tests regarding the data expected

	// All
	if (strncmp(json_object_get_string(arg), "all", 3) == 0) {
		json_object_object_add(res, "Day", json_object_new_int(instant.tm_mday));
		json_object_object_add(res, "Month", json_object_new_int(instant.tm_mon+1));
		json_object_object_add(res, "Year", json_object_new_int(instant.tm_year+1900));
		json_object_object_add(res, "Hour", json_object_new_int(instant.tm_hour));
		json_object_object_add(res, "Minute", json_object_new_int(instant.tm_min));
		json_object_object_add(res, "Second", json_object_new_int(instant.tm_sec));
		if(afb_event_is_valid(event_date) && instant.tm_sec % 2 == 0){
			json_object_object_add(res_event, "Day", json_object_new_int(instant.tm_mday));
			json_object_object_add(res_event, "Month", json_object_new_int(instant.tm_mon+1));
			json_object_object_add(res_event, "Year", json_object_new_int(instant.tm_year+1900));
			json_object_object_add(res_event, "Hour", json_object_new_int(instant.tm_hour));
			json_object_object_add(res_event, "Minute", json_object_new_int(instant.tm_min));
			json_object_object_add(res_event, "Second", json_object_new_int(instant.tm_sec));
			listenersDate = afb_event_push(event_date, res_event);
		}
	}

	// Day
	if (strncmp(json_object_get_string(arg), "day", 3) == 0) {
		json_object_object_add(res, "Day", json_object_new_int(instant.tm_mday));
	}

	// Month
	if (strncmp(json_object_get_string(arg), "month", 5) == 0) {
		json_object_object_add(res, "Month", json_object_new_int(instant.tm_mon+1));
	}

	// Year
	if (strncmp(json_object_get_string(arg), "year", 4) == 0) {
		json_object_object_add(res, "Year", json_object_new_int(instant.tm_year+1900));
	}

	// Hour
	if (strncmp(json_object_get_string(arg), "hour", 4) == 0) {
		json_object_object_add(res, "Hour", json_object_new_int(instant.tm_hour));
	}

	// Minute
	if (strncmp(json_object_get_string(arg), "minute", 6) == 0) {
		json_object_object_add(res, "Minute", json_object_new_int(instant.tm_min));
	}

	// Second
	if (strncmp(json_object_get_string(arg), "second", 6) == 0) {
		json_object_object_add(res, "Second", json_object_new_int(instant.tm_sec));
	}
	afb_req_success_f(request, res, "Data provided by C library, current listeners : %d", listenersDate);
	AFB_API_NOTICE(afbBindingV3root, "Verbosity macro at level notice invoked at date");
}

static void subscribe(afb_req_t request)
{
	// J-son argument
	json_object *args, *argDate, *argCount;

	// Information regarding the one who called the subscribe function
	char *usr;
	usr = afb_req_context_get(request);

	// J-son which gathered arguments in command line
	args = afb_req_json(request);
	AFB_REQ_NOTICE(request, "action n° %s: %s", usr, json_object_get_string(args));

	// Date
	if (json_object_object_get_ex(args, "date", &argDate)){

		// Subscription
		if (json_object_get_boolean(argDate)){
			AFB_REQ_NOTICE(request, "Date subscription succeed");
    		afb_req_subscribe(request, event_date);
		}

		// Unsubscription
		else{
			AFB_REQ_NOTICE(request, "Date unsubscription succeed");
    		afb_req_unsubscribe(request, event_date);
		}
	}

	// Count
	else if (json_object_object_get_ex(args, "count", &argCount)){

		// Subscription
		if (json_object_get_boolean(argCount)){
			AFB_REQ_NOTICE(request, "Count subscription succeed");
    		afb_req_subscribe(request, event_count);
		}

		// Unsubscription
		else{
			AFB_REQ_NOTICE(request, "Count unsubscription succeed");
    		afb_req_unsubscribe(request, event_count);
		}
	}

	// Invalid request
	else{
		afb_req_fail_f(request, "ERROR", "Invalid request in %s.", json_object_get_string(args));
		return;
	}

	afb_req_reply(request, json_object_get(args), NULL, NULL);
}

// Preinitialization
int preinit(afb_api_t api)
{
	AFB_API_NOTICE(api, "Preinitialization");
	return 0;
}

// Events initialization
int init(afb_api_t api)
{
	event_date = afb_api_make_event(api, "date");
	event_count = afb_api_make_event(api, "count");
	if (afb_event_is_valid(event_date) && afb_event_is_valid(event_count)){
		AFB_API_NOTICE(api, "Events initialization succeed");
		return 0;
	}
	AFB_API_ERROR(api, "Can't create events");
	return -1;
}

static const struct afb_auth _afb_auths_v2_monitor[] = {
	{.type = afb_auth_Permission, .text = "urn:AGL:permission:monitor:public:set"},
	{.type = afb_auth_Permission, .text = "urn:AGL:permission:monitor:public:get"},
	{.type = afb_auth_Or, .first = &_afb_auths_v2_monitor[1], .next = &_afb_auths_v2_monitor[0]}
};

static const afb_verb_t verbs[] = {
    {.verb = "count", .session = AFB_SESSION_NONE, .callback = testCount, .auth = NULL},
	{.verb = "date", .session = AFB_SESSION_NONE, .callback = timeDisp, .auth = NULL},
	{.verb = "subscribe", .session = AFB_SESSION_NONE, .callback = subscribe, .auth = NULL},
	{.verb = NULL}
};

const afb_binding_t afbBindingExport = {
	.api = "test",
	.specification = NULL,
	.verbs = verbs,
	.preinit = preinit,
	.init = init,
	.onevent = NULL,
	.userdata = NULL,
	.provide_class = NULL,
	.require_class = NULL,
	.require_api = NULL,
	.noconcurrency = 0
};
