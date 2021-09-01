% kardelen erdal
% 2018400024
% compiling: yes
% complete: yes

:- encoding(utf8).
% include the knowledge base
:- ['load.pro'].

% returns the distance from Name1 to Name2.
glanian_distance(Name1, Name2, Distance) :- 
	expects(Name1, _, Expected_list),
	glanian(Name2, _, Features),
	calculate_distance(Expected_list, Features, DistanceSquared),
	Distance is sqrt(DistanceSquared).

% if Name1 does not care about that feature, the sum is 0 for that feature.
calculate_distance([-1], _, Distance) :- Distance is 0.

% the actual calculation for distance.
calculate_distance([H1], [H2], Distance) :-
	Distance is (H1-H2)*(H1-H2).

% takes 2 lists as parameters, and one by one calculates the sums.
calculate_distance([H1|T1], [H2|T2], Distance) :-
	calculate_distance(T1, T2, TailDistance),
	calculate_distance([H1], [H2], HeadDistance),
	Distance is HeadDistance + TailDistance.

% returns the weighted distance from Name1 to Name2.
weighted_glanian_distance(Name1, Name2, Distance) :-
	expects(Name1, _, Expected_list),
	glanian(Name2, _, Features),
	weight(Name1, WeightList),
	calculate_weighted_distance(Expected_list, Features, WeightList, DistanceSquared),
	Distance is sqrt(DistanceSquared).

% if Name1 does not care about that feature, the sum is 0 for that feature.
calculate_weighted_distance([-1], _, [-1], Distance) :- 
	Distance is 0.

% the actual calculation for weighted distance.
calculate_weighted_distance([H1], [H2], [H3], Distance) :-
	(H1 =\= -1 , H3 =\= -1),
	Distance is (H1-H2)*(H1-H2)*H3.

% takes 2 lists as parameters, and one by one calculates the sums.
calculate_weighted_distance([H1|T1], [H2|T2], [H3|T3], Distance) :-
	calculate_weighted_distance(T1, T2, T3, TailDistance),
	calculate_weighted_distance([H1], [H2], [H3], HeadDistance),
	Distance is HeadDistance + TailDistance.

% returns Name's current city and her liked cities as a list.
find_possible_cities(Name, CityList) :- 
	likes(Name, _, LikedCities),
	find_current_city(Name, CurrentCity), 
	append([CurrentCity], LikedCities, CityList2),
	remove_duplicates(CityList2, CityList), !.

% returns Name's current city.
find_current_city(Name, City) :- 
	city(City, Citizens, _),
	member(Name, Citizens).

% returns the union of Name1's and Name2's possible cities.
merge_possible_cities(Name1, Name2, MergedCities) :- 
	find_possible_cities(Name1, CityList1), 
	find_possible_cities(Name2, CityList2), 
	append(CityList1, CityList2, MergedCitiesDuplicates), 
	remove_duplicates(MergedCitiesDuplicates, MergedCities), !.

% remove duplicate base condition.
remove_duplicates([], []).

% if the element is duplicated, removes it.
remove_duplicates([Head|Tail], NewTail) :- 
	member(Head, Tail), 
	remove_duplicates(Tail, NewTail).

% if the element is not duplicated, it stays.
remove_duplicates([Head|Tail], [Head|NewTail]) :- 
	not(member(Head, Tail)), 
	remove_duplicates(Tail, NewTail).

% returns mutual liked activities of Name1 and Name2.
find_mutual_activities(Name1, Name2, MutualActivities) :-
	likes(Name1, Activities1, _),
	likes(Name2, Activities2, _),
	find_mutuals(Activities1, Activities2, MutualActivities), !.

% find mutual base condition.
find_mutuals([], _, []).

% if an element is mutual, it appends.
find_mutuals([Head1|Tail1], List2, Intersection) :- 
	member(Head1, List2),
	find_mutuals(Tail1, List2, Intersection2),
	append([Head1], Intersection2, Intersection).

% if an element is not mutual, it is not in the list.
find_mutuals([Head1|Tail1], List2, Intersection) :- 
	not(member(Head1, List2)),
	find_mutuals(Tail1, List2, Intersection).

% finds glanians who are one of Name's expected gender and sorts them according to the distances.
find_possible_targets(Name, Distances, TargetList) :- 
	findall(TargetName, (glanian(TargetName, Gender, _), expects(Name, ExpectedGenderList, _), member(Gender, ExpectedGenderList), Name \= TargetName), TargetListUnsorted),
	find_possible_distances(Name, TargetListUnsorted, DistancesAndHeads),
	keysort(DistancesAndHeads, DistancesAndHeadsSorted),
	findall(X, member(X-_,DistancesAndHeadsSorted), Distances),
	findall(Y, member(_-Y,DistancesAndHeadsSorted), TargetList), 
	!.

% finds distances of a list of glanians from Name.
find_possible_distances(Name, [Head|Tail], Distances) :-
	glanian_distance(Name, Head, Distance),
	find_possible_distances(Name, Tail, TailDistance),
	append([Distance-Head], TailDistance, Distances).

% base condition.
find_possible_distances(_, [], []).

% finds glanians who are one of Name's expected gender and sorts them according to the weighted distances.
find_weighted_targets(Name, Distances, TargetList) :- 
	findall(TargetName, (glanian(TargetName, Gender, _), expects(Name, ExpectedGenderList, _), member(Gender, ExpectedGenderList), Name \= TargetName), TargetListUnsorted),
	find_possible_weighted_distances(Name, TargetListUnsorted, DistancesAndHeads),
	keysort(DistancesAndHeads, DistancesAndHeadsSorted),
	findall(X, member(X-_,DistancesAndHeadsSorted), Distances),
	findall(Y, member(_-Y,DistancesAndHeadsSorted), TargetList),
	!.

% finds weighted distances of a list of glanians from Name.
find_possible_weighted_distances(Name, [TargetHead|TargetTail], Distances) :-
	weighted_glanian_distance(Name, TargetHead, Distance),
	find_possible_weighted_distances(Name, TargetTail, TailDistance),
	append([Distance-TargetHead], TailDistance, Distances).

% base condition.
find_possible_weighted_distances(_, [], []).

% combine base condition.
mycombine([],[],[],[],[]).

% combines 4 lists into 1 list.
mycombine([Dh|Dt], [Th|Tt], [Ch|Ct], [Ah|At], AllLists) :-
	mycombine(Dt, Tt, Ct, At, AllListsTail),
	append([Dh-[Th, Ch, Ah]], AllListsTail, AllLists).

% divide base condition.
mydivide([], [], [], [], []).

% divides a list into 4 different lists.
mydivide([Distance-[Target, City, Activity]|At], Targets, Distances, Activities, Cities) :-
	mydivide(At, TargetsTail, DistancesTail, ActivitiesTail, CitiesTail),
	append([Target], TargetsTail, Targets),
	append([City], CitiesTail, Cities),
	append([Activity], ActivitiesTail, Activities),
	append([Distance], DistancesTail, Distances).

% returns list of targets, distances, activities and cities for a glanian.
find_my_best_target(Name, Distances, Activities, Cities, Targets) :-
	find_weighted_targets(Name, _, PossibleTargetList),
	find_targets(Name, PossibleTargetList, TargetList),
	find_cities_and_activities(Name, TargetList, Cities2, Activities2, Targets2),
	find_distances(Name, Targets2, Distances2),
	mycombine(Distances2, Targets2, Cities2, Activities2, AllListsUnsorted),
	keysort(AllListsUnsorted, AllLists),
	mydivide(AllLists, Targets, Distances, Activities, Cities),
	!.

% finds distances of a list of glanians from Name.
find_distances(Name, [Head|Tail], Distances) :-
	weighted_glanian_distance(Name, Head, Distance),
	find_distances(Name, Tail, TailDistance),
	append([Distance], TailDistance, Distances).

% base condition.
find_distances(_, [], []).	

% decrements.
decr(X,NX) :-
    NX is X-1.

% writes Name, Times times, into List.
write_x_times(Name, Times, List) :- 
	Times =\= 0,
	decr(Times, NewTimes),
	write_x_times(Name, NewTimes, List2),
	append([Name], List2, List).

% base condition.
write_x_times(_, 0, []).	

% base condition.
divide_city_activity([], [], []).

% divides a list into citylist and activitylist.	
divide_city_activity([Activity-City|CaTail], CityList, ActivityList) :-	
	divide_city_activity(CaTail, CityListTail, ActivityListTail),
	append([City], CityListTail, CityList),
	append([Activity], ActivityListTail, ActivityList).

% According to Name and Targets, finds cities and activities for them.
find_cities_and_activities(Name, [TargetHead|TargetTail], CityList, ActivityList, TargetsDuplicated) :-
	merge_possible_cities(Name, TargetHead, MergedCities),
	findall(Activity-City, (is_compatible(Name, Activity, City), member(City, MergedCities)), ActivityandCityHead2),
	keysort(ActivityandCityHead2, ActivityandCityHeadS),
	remove_duplicates(ActivityandCityHeadS, ActivityandCityHead),
	length(ActivityandCityHead, Times),
	write_x_times(TargetHead, Times, TargetsDuplicatedHead),
	divide_city_activity(ActivityandCityHead, CityListHead, ActivityListHead),
	find_cities_and_activities(Name, TargetTail, CityTail, ActivityTail, TargetsTail),
	append(TargetsDuplicatedHead, TargetsTail, TargetsDuplicated),
	append(CityListHead, CityTail, CityList),
	append(ActivityListHead, ActivityTail, ActivityList).

% base condition.
find_cities_and_activities(_, [], [], [], []).

% finds targets for Name.
find_targets(Name, PossibleTargetList, TargetList) :-
	expects(Name, ExpectedGenderList, _),
	findall(TargetName, (glanian(TargetName, Gender, _), member(TargetName, PossibleTargetList), member(Gender, ExpectedGenderList), not(find_old_relation(Name, TargetName)), in_tolerance_limits(Name, TargetName), activity_match(Name, TargetName, Conflict), Conflict < 3), TargetList2),
	remove_duplicates(TargetList2, TargetList).

% finds how many activities that conflicts in Name and Target.
activity_match(Name, TargetName, Conflict) :-
	dislikes(Name, DislikedActivities, _, _),
	likes(TargetName, LikedActivities, _),
	count_intersection(DislikedActivities, LikedActivities, Conflict),!.

% counts intersection in a list.
count_intersection([Head1|Tail1], List2, Count) :-
	member(Head1, List2), count_intersection(Tail1, List2, Count2), Count is 1+Count2.

% counts intersection in a list.
count_intersection([Head1|Tail1], List2, Count) :-
	not(member(Head1, List2)), count_intersection(Tail1, List2, Count).

% base conditions.
count_intersection([], _, 0).
count_intersection(_, [], 0).	

% returns true if two glanians have an old relation.
find_old_relation(Name, TargetName) :-
	old_relation([Name, TargetName]); old_relation([TargetName, Name]).

% returns true if targetname is in tolerance limits of name.
in_tolerance_limits(Name, TargetName) :-
	glanian(TargetName, _, FeaturesList),
	dislikes(Name, _, _, Limits),
	check_tolerance_limits(FeaturesList, Limits).

% checks tolerance limits given 2 lists.
check_tolerance_limits([FeaturesHead|FeaturesTail], [LimitsHead|LimitsTail]) :-
	(is_empty(LimitsHead) ; (getMinMaxLimits(LimitsHead, MinLimit, [MaxLimit]), FeaturesHead > MinLimit, FeaturesHead < MaxLimit)),
	check_tolerance_limits(FeaturesTail, LimitsTail).

% base condition.
check_tolerance_limits([], []).

% returns minlimit and maxlimit.
getMinMaxLimits([MinLimit|MaxLimit], MinLimit, MaxLimit).

% returns true if list is empty.
is_empty(List):- not(member(_,List)).

% returns list of matches, distances, activities and cities for a glanian.
find_my_best_match(Name, Distances, Activities, Cities, Targets) :-
	find_weighted_targets(Name, _, PossibleTargetList),
	find_matches(Name, PossibleTargetList, TargetList),
	find_cities_and_activities_matches(Name, TargetList, Cities2, Activities2, Targets2),
	find_distances_matches(Name, Targets2, Distances2),
	mycombine(Distances2, Targets2, Cities2, Activities2, AllListsUnsorted),
	keysort(AllListsUnsorted, AllLists),
	mydivide(AllLists, Targets, Distances, Activities, Cities),
	!.

% finds matches for Name.
find_matches(Name, PossibleTargetList, TargetList) :-
	glanian(Name, MyGender, _),
	expects(Name, ExpectedGenderList, _),
	findall(TargetName, (member(TargetName, PossibleTargetList), glanian(TargetName, Gender, _), member(Gender, ExpectedGenderList), expects(TargetName, ExpextedGenderList2, _), member(MyGender, ExpextedGenderList2), not(find_old_relation(Name, TargetName)), in_tolerance_limits(Name, TargetName), in_tolerance_limits(TargetName, Name), activity_match(TargetName, Name, Conflict2), Conflict2 < 3, activity_match(Name, TargetName, Conflict), Conflict < 3), TargetList2), 
	remove_duplicates(TargetList2, TargetList).

% returns true if Name can do Activity in City. 
is_compatible(Name, Activity, City):-
	(city(City, Habitants, ActivitiesInCity),
		likes(Name, _, LikedCities),
		dislikes(Name, DislikedActivities, DislikedCities, _),
		(member(Name, Habitants) ; (member(City, LikedCities))), member(Activity, ActivitiesInCity), not(member(Activity, DislikedActivities)), not(member(City, DislikedCities)));
	(dislikes(Name, _, DislikedCities2, _),
		likes(Name, LikedActivities2, _),
		city(City, _, ActivitiesInCity), not(member(City, DislikedCities2)), member(Activity, ActivitiesInCity), member(Activity, LikedActivities2)).	

% finds cities and activities given Name and TargetList.
find_cities_and_activities_matches(Name, [TargetHead|TargetTail], CityList, ActivityList, TargetsDuplicated) :-
	merge_possible_cities(Name, TargetHead, MergedCities),
	findall(Activity-City, (is_compatible(Name, Activity, City), is_compatible(TargetHead, Activity, City), member(City, MergedCities)), ActivityHeadAndCity2),
	keysort(ActivityHeadAndCity2, ActivityHeadAndCityS),
	remove_duplicates(ActivityHeadAndCityS, ActivityHeadAndCity),
	length(ActivityHeadAndCity, Times),
	write_x_times(TargetHead, Times, TargetsDuplicatedHead),
	divide_city_activity(ActivityHeadAndCity, CityListHead, ActivityListHead),
	find_cities_and_activities_matches(Name, TargetTail, CityTail, ActivityTail, TargetsTail),
	append(TargetsDuplicatedHead, TargetsTail, TargetsDuplicated),
	append(CityListHead, CityTail, CityList),
	append(ActivityListHead, ActivityTail, ActivityList).

% base condition.
find_cities_and_activities_matches(_, [], [], [], []).	

% finds distances for matches and Name.
find_distances_matches(Name, [Head|Tail], Distances) :-
	weighted_glanian_distance(Name, Head, Distance1),
	weighted_glanian_distance(Head, Name, Distance2),
	find_distances_matches(Name, Tail, TailDistance),
	Distance is (Distance1 + Distance2)/2,
	append([Distance], TailDistance, Distances).

% base condition.
find_distances_matches(_, [], []).	
