# ISOM4300 Assignment 1

> by Aleksandr Sergeev, student number: `21158862`.

## Question 1

1. The possible threats discovery.  
    From business perspective:
    1. **Food delivery reliability issues**  
        The food is being delivered on a daily basis, the level of supply varies every day.
        In case any unexpected trafficing delays appear, the business may suffer from product shortage once the previously stored supplies expire.
    2. **Requirement of manual order processing**  
        Every online delivery is being arranged by phone.
        This introduces a possibility for human miscommunication or misunderstanding happening under way.
        Moreover, every online food order is being recorded in the digital inventory manually once it is packed and delivered.
        This can be error-prone and require additional storage team effort.
    3. **Inventory inconsistency due to online orders**  
        The online food orders are not immediately reflected in the digital inventory system: it takes some time to send email, put the goods on hold and arrange delivery by phone.
        A customer might order a specific good that has already been held, but that change of state was not reflected in the inventory (and thus in online store) yet.

    Related to online store and IT insfrastructure:
    1. **Possible system outdating**
        The online store app was developed in 2018, that is 6 years ago, before Covid19 outbreak and online shopping popularity growth.
        It is likely that the application should be updated to the latest industry standards to reflect modern customer demand and security measures.
        Another risk comes along with this one: if something happens to R-iPro, it might be hard to find another company willing to support such an old proprietary online shopping system.
    2. **Online store serving device**  
        The whole online store depends on a single desktop-grade machine inside of the headquarter office.
        That might create a potential danger of the whole system going down without having a proper backup options.
        Moreover, normally desktop-grade machines are not suitable for a web application serving, they might be voulnerable, have insufficient network resources and require additional maintenance.
    3. **Lack of IT support**  
        That is understandable that having only 15 people employed, R-iPro company can not guarantee 24/7 support for their product, still, however, whatever they do provide seems to be unacceptable.
        If a system goes down at 5pm on Friday, it would take more than 65 hours to restore it.
        In addition to that, weekends and holidays may be the time of the greatest food delivery demand indeed, so the total support quality is not enough for a production-grade online store system.

2. Here is a classification table of risks described above:

    | Risk name | Risk type |
    | --- | --- |
    | Food delivery reliability issues | Natural (or a VERY large scale technical failure) |
    | Requirement of manual order processing | Man-made |
    | Inventory inconsistency due to online orders | Technical |
    | Possible system outdating | Man-made or technical |
    | Online store serving device | technical |
    | Lack of IT support | Man-made |

3. Risk impact/likelihood prediction:

    | Risk name | Impact level | Occurance probability | Justification |
    | --- | --- | --- | --- |
    | Food delivery reliability issues | Major (5) | Very unlikely (1) | Lack of products could result in both client and reputational losses, however since Japan is not so far from HK unavailability of both sea and air transportation seems to be unlikely |
    | Requirement of manual order processing | Minor (2) | Probable (5) | Even though humans tend to make mistakes often, the consequances of one particular order mistake can be considered as neglegible (especially if the order price is compensated to the client) |
    | Inventory inconsistency due to online orders | Moderate (3) | Likely (4) | This is a race condition risk, it will occure the more often the more orders are processed by the online store; it can impact more than one order and so be either expensive or result in client dissatisfaction |
    | Possible system outdating | Major (5) | Unlikely (2) | Although this risk can seem to be neglegible right now (while everything works and R-iPro supports its' product), it might suddenly result in whole online store loss once online store is either can not support increased customer demand anymore, is influenced by a cyberattack or can not be supported anymore |
    | Online store serving device | Major (5) | Likely (4) | Desktop-grade machines are normally not suitable for running a whole online store server, it can break down from time to time; such a break down can result in huge maintenance downtime of the store, big reputational, client and data losses |
    | Lack of IT support | Major (5) | Possible (3) | This issue is much related to the previous one; lack of support can lead to considerable online store downtimes and losses, that can happen when the digital system breaks down because of the short R-iPro working hours |

4. According to the risk assessment matrix that we used in course, the risk ratings can be calculated using the following formula: `Risk Rating = Probability x Impact`.
    Risk rating calculations and some proposed controls for the risks mentioned above can be found in the table below:

    | Risk name | Risk rating | Proposed controls |
    | --- | --- | --- |
    | Food delivery reliability issues | Low ($5 \cdot 1 = 5$) | Additional storage facilities might be built, local products developed by Japanese recepies can be used as a substitution |
    | Requirement of manual order processing | Medium ($2 \cdot 5 = 10$) | A digital system for order processing (that can also cooperate with courrier services) might be introduced, as a partial alternative an every-evening inventory checkup can be introduced |
    | Inventory inconsistency due to online orders | Medium ($3 \cdot 4 = 12$) | Automating the whole ordering process with immediate and atomic state changes could solve this issue, another approach could be always having a considerable surplus of the goods in stock |
    | Possible system outdating | Medium ($5 \cdot 2 = 10$) | Regular system updates could be used to bring the system to the newest standard and delegating the code to an escrow agency can reduce bancrupcy risk; another solution would be migrating to some open-source online store project, there should be some of them available since an online shop is a common service nowadays |
    | Online store serving device | High ($5 \cdot 4 = 20$) | The store serving should be transferred to a remote production-grade cloud center, also a backup of all data should be created regularly |
    | Lack of IT support | High ($5 \cdot 3 = 15$) | One of the solutions could be migration to another digital support provider, another could be hiring a system administrator, proficient with the system and responsible for supporting the online store any time it's needed |

5. Classification of the controls mentioned above can be found in the table below:

    | Control name | Control type | Description |
    | --- | --- | --- |
    | Building new storage facilities | preventive | Storing additional food helps not to run out of goods |
    | Local products usage | corrective | Even if the store has run out of the original Japanese goods, it won't stop working, but the other goods would be used instead |
    | Digital order processing system | preventive | Automated system helps avoiding human mistakes |
    | Daily inventory checkup | detective | checkup helps detecting mistakes in inventory records and correct them before it becomes a problem |
    | Order automatization | preventive | Automated system is capable of performing atomic operations and avoiding the inventory inconsistencies |
    | Good surplus storing | preventive | Always storing additional amount of goods can resolve issues caused by overbooking |
    | Regular system updates | preventive | Keeping system up-to-date can help improving its performance and protect against latest cyber attaks |
    | Delegating code to an escrow agency | corrective | Even if the current proprietary code owner goes bankrupt and can not support the online store anymore, the code is not lost and can be delegated to another IT company |
    | Using an open-source online store app | preventive | Open-source code is often updated by community members and can be supported by most of the developers, so it will never be lost |
    | Transferring hosting to a remote datacenter | preventive | Keeping the hosting online and preventing hardware failures is outsourced to the professionals, also any demand increase can be swiftly managed by using smart on-demand hosting plans |
    | Creating regular backups | corrective | Even if the store goes down and some data gets lost, the backup can always be restored and major losses prevented |
    | Migrating to another digital support provider | preventive | Using service provided by another support company with more flexible working hours can decrease potential store downtime and reduce losses |
    | Hiring a system administrator | preventive | Some of the online store issues (that can be solved locally) can be resolved by a local system administrator, available 24/7 to prevent store downtimes |

6. Risk re-evaluation might be necessary in one of these situations:
    1. After some considerable amount of time passes (e.g. 1 year).
    2. After any of the business processes is changed (e.g. proposed controls are implemented) or any other external events happen (e.g. change of governmental regulations or business environment).

7. Although it is never possible to eliminate all the risks completely, I believe that after (at least some of the) proposed controls are implemented, the risk ratings can decrease considerably.
    In the table below the updated risk likelihood, impact and rating can be found:

    | Risk name | Impact level | Occurance probability | Risk rating | Comment |
    | --- | --- | --- | --- | --- |
    | Food delivery reliability issues | Major (5) | Very unlikely (1) | Low ($5 \cdot 1 = 5$) | Event though this risk already was very unlikely, using a local product supply chain makes it completely neglegible |
    | Requirement of manual order processing | Minor (2) | Very unlikely (1) | Low ($2 \cdot 1 = 2$) | Once the ordering system is automated, human mistakes can be almost completely avoided |
    | Inventory inconsistency due to online orders | Moderate (3) | Very unlikely (1) | Low ($3 \cdot 1 = 3$) | If the ordering system becomes atomic, inventory inconsistency becomes highly unlikely and maybe a human factor only |
    | Possible system outdating | Major (5) | Unlikely (2) | Medium ($5 \cdot 2 = 10$) | Even though cyber attaks and systems breakdowns happen to all IT products, keeping them up-to-date guarantees the best defence levels available right now (not absolute however) |
    | Online store serving device | Major (5) | Very unlikely (1) | Low ($5 \cdot 1 = 5$) | Big cloud datacenters provide users with 99.999% uptime, making system subject tohardware failures really rarely |
    | Lack of IT support | Major (5) | Unlikely (2) | Medium ($5 \cdot 2 = 10$) | No matter how well system is supported, fixing failures may take time; still if the issue is being identified and started to be solved right away, it can reduce negative customer experiences |

## Question 2

1. That friend of mine can be interested in several areas of JPLine Inc. business.
    For example, these areas could be online shop cybersecurity and customer information safety, HK an Japanese governmental regulations, supply chain voulnerabilities, outsource service providers, etc.
    In order to understand better about what is relevant for the particular audit assignment, my friend could conduct an on-site business process review, read the documentation of the software used, ask senior management of JPLine Inc. about their business, what do they do and what challenges they meet, and also ask the CEO of HJ Food Inc. about what business areas exactly they are interested in.

2. Not many of the controls that I have suggested above require some policy introduction.
    Still, successful introduction of the controls can be checked, in the table below I have put some of my ideas about it:

    | Control name | Compilance check idea |
    | --- | --- |
    | Building new storage facilities | Storage facilities can be inspected |
    | Local products usage | Contracts with the local suppliers can be inspected, quality standards of locally-produced goods can be checked |
    | Digital order processing system | Audit records of the automated system can be verified |
    | Daily inventory checkup | Checkup logs and notes should be validated |
    | Order automatization | The order logs can be automatically checked for inconsistencies that may appear due to the processes being non-atomic |
    | Good surplus storing | The number of items in stock over the last few months might be validated |
    | Regular system updates | The date of the last update can be checked |
    | Delegating code to an escrow agency | Contract with the agency can be verified, the agency representative can be asked about the contract validity |
    | Using an open-source online store app | The open repository of the app should be checked, the repository owner can be contacted in order to verify if the project is still actively supported |
    | Transferring hosting to a remote datacenter | Benchmark can be run, uptime logs can be checked |
    | Creating regular backups | Backup history and consistency can be checked |
    | Migrating to another digital support provider | Support provider representative can be enquired about their working hours |
    | Hiring a system administrator | Log of administrator account actions can be checked |

3. Some of the compliance checks (and especially the ones that failed) could be backed up by the substantive tests.
    Speaking about integrity of the storage system, here are the few areas that could be examined:

    1. In order to check the automated order making system, some error-prone or confusing for humans orders can be made.
    2. For inventory consistency checking, an attempt can be made to order the same item twice simultaneously (it should fail).
    3. For verifying latest system updates, the whole app can be searched for presence of the few lately-discovered major programming language, platform or library bugs.
    4. In case open-source app is used, the repository `issues` tab can be studied, the latest changes introduced should be checked.
    5. While testing remote hosting provider, access time from different geographic locations of the city can be verified.
    6. An attempt to restore a backup image can be made (it should be successful and all the data records should be preserved).
    7. App support center representative can be asked to provide a sample support task late at night or on holiday to check response time.
    8. System administrator can be asked to pass a qualification test.

4. Control self assessment can be performed by JPLine Inc. in a several different ways.
    Probably it would be easier and safer for the staff to focus on simple everyday risks than trying to check some major disasters like an unexpected system hardware failure.
    For examples the following risks can be checked by JPLine Inc. employees on regular basis:

    1. **System benchmarks**  
        System benchmarks and load tests can be run from time to time (e.g. at night) to check operational state and average response time.
    2. **Support responsiveness**  
        The company responsible for the online store support can be contacted from time to time to ensure they are online and ready to help.
    3. **Emergency workshops**  
        Emergency workshops and tutorials can be conducted from time to time to ensure employees are proparerd for a stressful emergency situation and know for sure what should they do.
    4. **Inventorization checks**  
        All the physical storage facilities should be inspected regularily to ensure all the products are kept in good conditions and not expired yet.
    5. **Simple questionnaries**  
        All the employees can be asked to answer a few simple questions once in a while to ensure they are familiar with company standards and policies.

5. A professional auditor should only report to the senior management of the company that hired them.
    However, during the follow-up period, auditor can also help the managers in charge of the company departments where any risks were identified to mitigate the risks and introduce the proposed controls correctly.
    Still, all the decision and change making is the area of responsibility of the senior management, auditor creates a report only and gives advices.
