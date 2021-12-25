from pages._xpath import Tag, Param, Condition
import pages


def _check_login(method):
    def wrapper(self, *args, **kwargs):
        if not self._username:
            raise RuntimeError(f"No user is logged in! Call 'login()' first!")
        return method(self, *args, **kwargs)
    return wrapper


def _check_repos_loaded(method):
    def wrapper(self, *args, **kwargs):
        self._await_element_to_appear(Tag.Div, (Param.Id, Condition.Equals, "repos-container"))
        return method(self, *args, **kwargs)
    return wrapper


class MainPage(pages.BasePage):
    def __init__(self, driver=None, username=None, *args, **kwargs):
        super(MainPage, self).__init__(driver, username, "", *args, **kwargs)

    def login(self) -> pages.LoginPage:
        if self._username:
            raise RuntimeError(f"Can not log in because you have been already logged in (username: '{self._username}')! Call 'logout()' first!")
        self._navigate("/login")
        return pages.LoginPage(self._driver)

    @_check_login
    def logout(self) -> pages.MainPage:
        self._find_visible_enabled(Tag.Summary, (Param.Label, Condition.Contains, "View profile and more")).click()
        self._await_element_to_appear(Tag.Button, (Param.Text, Condition.Contains, "Sign out")).click()
        return pages.MainPage(self._driver)

    @_check_login
    @_check_repos_loaded
    def new_repo(self) -> pages.NewPage:
        self._navigate("/new")
        return pages.NewPage(self._driver, self._username)

    @_check_login
    @_check_repos_loaded
    def has_repo(self, repo_name: str) -> bool:
        return self._find_visible_enabled(Tag.Link, (Param.Link, Condition.Contains, f"/{self._username}/{repo_name}")) is not None

    @_check_login
    @_check_repos_loaded
    def open_repo(self, repo_name: str) -> pages.RepoPage:
        self._navigate(f"/{self._username}/{repo_name}")
        return pages.RepoPage(self._driver, self._username, repo_name)

    @_check_login
    @_check_repos_loaded
    def repo_number(self) -> int:
        return len(self._find_children_visible_enabled(self._find_visible_enabled(Tag.List, (Param.DataIterable, Condition.Equals, "dashboard-repos-filter-left")), Tag.Elem))
